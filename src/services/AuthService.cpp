#include "services/AuthService.hpp"
#include "services/details/UserService.hpp"
#include "worktime_postgres_service/sql_queries.hpp"

#include <userver/formats/json/value_builder.hpp>
#include <userver/http/common_headers.hpp>
#include <userver/server/http/http_method.hpp>

using namespace userver;
using namespace services::auth;

AuthService::AuthService (
    const userver::components::ComponentConfig &config,
    const userver::components::ComponentContext &component_context)
    : control_role::details::UserService (component_context, "db"),
      userver::server::handlers::HttpHandlerJsonBase (config,
                                                      component_context)
{
}

AuthService::Value
AuthService::HandleRequestJsonThrow (const HttpRequest &request,
                                     const Value &request_json,
                                     RequestContext &context) const
{
  std::string action = request.GetPathArg ("action");
  auto handlers = std::unordered_map<std::string, std::function<Value ()>>{
    { "login",
      [&request, &request_json, &context, this] () -> Value {
        return HandleLoginRequestJsonThrow (request, request_json, context);
      } },
    { "logout",
      [&request, &request_json, &context, this] () -> Value {
        return HandleLogoutRequestJsonThrow (request, request_json, context);
      } }
  };

  auto handlerIt = handlers.find (action);
  if (handlerIt == handlers.end ())
    {
      throw ClientError (
          ExternalBody{ fmt::format ("Unprocessable action: {}", action) });
    }
  return handlerIt->second ();
}

AuthService::Value
AuthService::HandleLoginRequestJsonThrow (const HttpRequest &request,
                                          const Value &request_json,
                                          RequestContext &context) const
{
  if (request.GetMethod () != userver::v2_15::server::http::HttpMethod::kGet)
    {
      throw ClientError (ExternalBody{ "Unsupported method" });
    }
  auto b = formats::json::ValueBuilder{};
  b["action"] = "login";
  return b.ExtractValue ();
}

AuthService::Value
AuthService::HandleLogoutRequestJsonThrow (const HttpRequest &request,
                                           const Value &request_json,
                                           RequestContext &) const
{
  if (request.GetMethod () != userver::v2_15::server::http::HttpMethod::kGet)
    {
      throw ClientError (ExternalBody{ "Unsupported method" });
    }
  auto b = formats::json::ValueBuilder{};
  b["action"] = "logout";
  return b.ExtractValue ();
}
