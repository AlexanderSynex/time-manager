#include "services/auth/AuthBearerChecker.hpp"

#include <string>
#include <string_view>

using namespace services::auth::schemas;

AuthBearerChecker::AuthBearerChecker (
    const services::caching::AuthCache &auth_cache,
    std::vector<userver::server::auth::UserScope> required_scopes)
    : auth_cache_ (auth_cache), required_scopes_{ required_scopes }
{
}

[[nodiscard]] userver::server::handlers::auth::AuthCheckResult
AuthBearerChecker::CheckAuth (
    const userver::server::http::HttpRequest &request,
    userver::server::request::RequestContext &) const
{
  auto authHeader = request.GetHeader (userver::http::headers::kAuthorization);
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

  const auto authSchemaPos = authHeader.find (' ');
  if (authSchemaPos == std::string::npos
      or std::string_view{ authHeader.data (), authSchemaPos } != "Bearer")
    {
      return userver::server::handlers::auth::AuthCheckResult{
        userver::server::handlers::auth::AuthCheckResult::Status::
            kTokenNotFound,
        {},
        "Invalid header format",
        userver::server::handlers::HandlerErrorCode::kUnauthorized
      };
    }

  const auto token
      = userver::server::auth::UserAuthInfo::Ticket{ authHeader.data ()
                                                     + authSchemaPos + 1 };
  const auto snapshot = auth_cache_.Get ();
  auto it = snapshot->find (token);
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
  return {};
}
