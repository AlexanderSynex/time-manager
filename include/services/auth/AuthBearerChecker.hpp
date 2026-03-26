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
class AuthBearerChecker final
    : public userver::server::handlers::auth::AuthCheckerBase
{
public:
  using AuthCheckResult = userver::server::handlers::auth::AuthCheckResult;

  AuthBearerChecker (
      const services::caching::AuthCache &auth_cache,
      std::vector<userver::server::auth::UserScope> required_scopes);

  [[nodiscard]] userver::server::handlers::auth::AuthCheckResult
  CheckAuth (const userver::server::http::HttpRequest &request,
             userver::server::request::RequestContext &) const override;
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
