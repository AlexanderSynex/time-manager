#pragma once

#include <map>
#include <string>
#include <string_view>
#include <userver/components/raw_component_base.hpp>
#include <userver/http/status_code.hpp>
#include <userver/logging/log.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/server/http/http_method.hpp>
#include <userver/server/middlewares/http_middleware_base.hpp>
#include <userver/yaml_config/yaml_config.hpp>

namespace services::middleware
{
struct CorsMiddleware final
    : public userver::server::middlewares::HttpMiddlewareBase
{
public:
  constexpr static std::string_view kName = "cors-middleware-service";

  CorsMiddleware (const userver::server::handlers::HttpHandlerBase &,
                  userver::yaml_config::YamlConfig config)
      : origins{ config["allow-origins"].As<std::string> ("*") },
        methods{ config["allow-methods"].As<std::string> (
            "GET, POST, PUT, DELETE") },
        headers{ config["allow-headers"].As<std::string> (
            "Content-Type, Authorization, Origin") },
        credentials{ config["allow-credentials"].As<std::string> ("true") }
  {
  }

  void
  HandleRequest (
      userver::server::http::HttpRequest &request,
      userver::server::request::RequestContext &context) const override
  {
    constexpr std::string_view methodNamePrefix = "Access-Control-Allow-";
    auto &responce = request.GetHttpResponse ();

    for (auto [method, value] :
         std::map<std::string, std::string>{ { "Origin", origins },
                                             { "Methods", methods },
                                             { "Headers", headers },
                                             { "Credentials", credentials } })
      {
        responce.SetHeader (std::string{ methodNamePrefix } + method, value);
      }
    if (request.GetMethod ()
        == userver::v2_15::server::http::HttpMethod::kOptions)
      {
        responce.SetStatus (userver::v2_15::http::kOk);
        return;
      }
    Next (request, context);
  }

private:
  std::string origins, methods, headers, credentials;
};
}
