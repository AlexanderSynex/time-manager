#pragma once

#include "info/Worker.hpp"
#include "services/details/UserService.hpp"
#include <string_view>
#include <userver/components/component_config.hpp>
#include <userver/components/component_context.hpp>
#include <userver/server/handlers/http_handler_json_base.hpp>
#include <userver/server/http/http_request.hpp>

namespace services::auth
{

class AuthService final : public control_role::details::UserService,
                          public userver::server::handlers::HttpHandlerJsonBase
{
public:
  static constexpr std::string_view authSchema = "Bearer";
  static constexpr std::string_view kName = "auth-service";

  explicit AuthService (
      const userver::components::ComponentConfig &config,
      const userver::components::ComponentContext &component_context);

  bool
  checkLogin () const
  {
    return true;
  }

  bool validateCredentials (std::string_view login,
                            std::string_view password) const;

  Value HandleRequestJsonThrow (const HttpRequest &request,
                                const Value &request_json,
                                RequestContext &) const override;

  Value HandleLoginRequestJsonThrow (const HttpRequest &request,
                                     const Value &request_json,
                                     RequestContext &) const;

  Value HandleLogoutRequestJsonThrow (const HttpRequest &request,
                                      const Value &request_json,
                                      RequestContext &) const;
};
}
