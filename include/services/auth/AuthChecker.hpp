#pragma once

#include "caching/AuthPolicy.hpp"

#include <userver/http/common_headers.hpp>
#include <userver/server/auth/user_auth_info.hpp>
#include <userver/server/auth/user_scopes.hpp>
#include <userver/server/handlers/auth/auth_checker_base.hpp>
#include <userver/server/http/http_request.hpp>
#include <userver/server/request/request_context.hpp>

#include <vector>

namespace services::auth::schemas
{
template <typename AuthSchema>
class AuthChecker final
    : public userver::server::handlers::auth::AuthCheckerBase
{
public:
  using AuthCheckResult = userver::server::handlers::auth::AuthCheckResult;

  AuthChecker (const services::caching::AuthCache &auth_cache,
               std::vector<userver::server::auth::UserScope> required_scopes)
      : auth_cache_ (auth_cache), required_scopes_{ required_scopes }
  {
  }

  [[nodiscard]] userver::server::handlers::auth::AuthCheckResult
  CheckAuth (
      const userver::server::http::HttpRequest &request,
      userver::server::request::RequestContext &request_context) const override
  {
    auto authHeader
        = request.GetHeader (userver::http::headers::kAuthorization);
    if (authHeader.empty ())
      {
        return userver::server::handlers::auth::AuthCheckResult{
          userver::server::handlers::auth::AuthCheckResult::Status::
              kTokenNotFound,
          {},
          "'Authorization' header is empty",
          userver::server::handlers::HandlerErrorCode::kUnauthorized
        };
      }

    auto token = AuthSchema::extractToken (authHeader);

    const auto authSchemaPos = authHeader.find (' ');
    if (not token.has_value ())
      {
        return userver::server::handlers::auth::AuthCheckResult{
          userver::server::handlers::auth::AuthCheckResult::Status::
              kTokenNotFound,
          {},
          "Invalid header format",
          userver::server::handlers::HandlerErrorCode::kUnauthorized
        };
      }

    const auto tokenTicket
        = userver::server::auth::UserAuthInfo::Ticket{ authHeader.data ()
                                                       + authSchemaPos + 1 };
    const auto snapshot = auth_cache_.Get ();
    auto it = snapshot->find (tokenTicket);
    if (it == snapshot->end ())
      {
        return AuthCheckResult{ AuthCheckResult::Status::kForbidden,
                                "Token invalid" };
      }

    const services::caching::AuthInfo &info = it->second;

    for (auto scope : required_scopes_)
      {
        if (std::find (info.scopes.begin (), info.scopes.end (), scope)
            == info.scopes.end ())
          {
            return AuthCheckResult{ AuthCheckResult::Status::kForbidden,
                                    fmt::format ("No scope '{}' permission",
                                                 scope.GetValue ()) };
          }
      }
    request_context.SetData ("table_id", info.table_id);
    return {};
  }

  [[nodiscard]] bool
  SupportsUserAuth () const noexcept override
  {
    return true;
  }

private:
  const services::caching::AuthCache &auth_cache_;
  const std::vector<userver::server::auth::UserScope> required_scopes_;
};

}
