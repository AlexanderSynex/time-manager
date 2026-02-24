#include "services/WorktimeService.hpp"
#include "services/details/UserService.hpp"

#include <userver/components/component_config.hpp>
#include <userver/components/component_context.hpp>
#include <userver/formats/json/value_builder.hpp>
#include <userver/formats/yaml/value.hpp>
#include <userver/logging/log.hpp>
#include <userver/server/handlers/exceptions.hpp>
#include <userver/server/handlers/http_handler_json_base.hpp>
#include <userver/server/http/http_method.hpp>

using namespace userver;
using namespace userver::server;
using namespace services::control_role;

WorktimeService::WorktimeService(
    const components::ComponentConfig& config,
    const components::ComponentContext& component_context)
    : details::UserService(component_context, "db")
    , handlers::HttpHandlerJsonBase(config, component_context)
{
}
handlers::HttpHandlerJsonBase::Value
WorktimeService::HandleRequestJsonThrow(const HttpRequest& request, const Value& request_json, RequestContext&) const
{
    switch (request.GetMethod()) {
    case userver::server::http::HttpMethod::kPost:
        return request_json;
    case userver::server::http::HttpMethod::kPut:
        return request_json;
    case userver::server::http::HttpMethod::kGet:
        return request_json;
    default:
        throw server::handlers::ClientError(server::handlers::ExternalBody {
            fmt::format("Unsupported method {}", request.GetMethod()) });
    }
    return request_json;
}
server::handlers::HttpHandlerJsonBase::Value WorktimeService::workerArrived(Worker&& worker)
{

    return formats::json::ValueBuilder {}.ExtractValue();
}
server::handlers::HttpHandlerJsonBase::Value WorktimeService::workerDeparted(Worker&&)
{
    return formats::json::ValueBuilder {}.ExtractValue();
}
