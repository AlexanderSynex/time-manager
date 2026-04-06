#pragma once

#include "CorsMiddleware.hpp"
#include <memory>
#include <string_view>
#include <userver/components/raw_component_base.hpp>
#include <userver/formats/yaml.hpp>
#include <userver/formats/yaml/serialize.hpp>
#include <userver/http/status_code.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/server/http/http_method.hpp>
#include <userver/server/middlewares/http_middleware_base.hpp>
#include <userver/yaml_config/schema.hpp>
#include <userver/yaml_config/yaml_config.hpp>

namespace services::middleware
{
struct CorsMiddlewareFactory final
    : public userver::server::middlewares::HttpMiddlewareFactoryBase
{
public:
  constexpr static std::string_view kName{ CorsMiddleware::kName };
  using userver::server::middlewares::HttpMiddlewareFactoryBase::
      HttpMiddlewareFactoryBase;

  std::unique_ptr<userver::server::middlewares::HttpMiddlewareBase>
  Create (const userver::server::handlers::HttpHandlerBase &request,
          userver::yaml_config::YamlConfig config) const override
  {
    return std::make_unique<CorsMiddleware> (request, std::move (config));
  }

  static userver::yaml_config::Schema
  GetStaticConfigSchema ()
  {
    return userver::formats::yaml::FromString (R"(
type: object
description: Config for this particular middleware
additionalProperties: false
properties:
    allow-origins:
        type: string
        description: Access-Control-Allow-Origins
    allow-methods:
        type: string
        description: Access-Control-Allow-Methods
    allow-headers:
        type: string
        description: Access-Control-Allow-Headers
    allow-credentials:
        type: string
        description: Access-Control-Allow-Credentials
)")
        .As<userver::yaml_config::Schema> ();
  }
};
}
