#include "services/AdministrationService.hpp"
#include <userver/formats/json/value_builder.hpp>
#include <userver/server/http/http_method.hpp>

using namespace userver;
using namespace userver::formats::json;
using namespace services::control_role;

Value AdministrationService::HandleRequestJsonThrow(const HttpRequest& request,
    const Value& request_json,
    RequestContext&) const
{
    switch (request.GetMethod()) {
    case server::http::HttpMethod::kPost:
        return processCreation(std::move(request), std::move(request_json));
    case server::http::HttpMethod::kPut:
        return processModification(std::move(request), std::move(request_json));
    default:
        throw server::handlers::ClientError(server::handlers::ExternalBody {
            fmt::format("Unsupported method {}", request.GetMethod()) });
    }
}

bool services::control_role::AdministrationService::validateJson(const Value& request_json) const
{
    return request_json.HasMember("name") and request_json.HasMember("surname") and request_json.HasMember("table_id");
}

userver::server::handlers::HttpHandlerJsonBase::Value
services::control_role::AdministrationService::processCreation(
    const HttpRequest&, const Value& request_json) const
{
    ValueBuilder builder;
    builder["action"] = "Creating";
    builder["ok"] = validateJson(request_json);
    return builder.ExtractValue();
}
userver::server::handlers::HttpHandlerJsonBase::Value
services::control_role::AdministrationService::processModification(
    const HttpRequest&, const Value& request_json) const
{
    ValueBuilder builder;
    builder["action"] = "Updating";
    builder["ok"] = validateJson(request_json);
    return builder.ExtractValue();
}
