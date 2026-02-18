#include "services/AdministrationService.hpp"
#include "info/worker.hpp"

#include <userver/components/component_config.hpp>
#include <userver/components/component_context.hpp>
#include <userver/storages/postgres/component.hpp>

#include <userver/formats/json/value_builder.hpp>
#include <userver/server/handlers/http_handler_json_base.hpp>
#include <userver/server/http/http_method.hpp>
#include <userver/storages/postgres/cluster.hpp>

using namespace userver;
using namespace userver::formats::json;
using namespace services::control_role;

AdministrationService::AdministrationService(const components::ComponentConfig& config,
    const components::ComponentContext& component_context)
    : userver::server::handlers::HttpHandlerJsonBase::HttpHandlerJsonBase(config, component_context)
    , p_cluser(component_context.FindComponent<components::Postgres>("db").GetCluster())
{
}

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

bool AdministrationService::validateJson(const Value& request_json) const
{
    return request_json.HasMember(Worker::name_key) and request_json.HasMember(Worker::surname_key) and request_json.HasMember(Worker::table_key);
}

Worker AdministrationService::extractWorkerInfo(const Value& request_json) const
{
    if (not request_json[Worker::name_key].IsString())
        throw server::handlers::ClientError(server::handlers::ExternalBody {
            fmt::format("Unsupported type for {} (should be: {})", Worker::name_key, "string") });

    if (not request_json[Worker::surname_key].IsString())
        throw server::handlers::ClientError(server::handlers::ExternalBody {
            fmt::format("Unsupported type for {} (should be: {})", Worker::surname_key, "string") });

    if (not request_json[Worker::table_key].IsNumber())
        throw server::handlers::ClientError(server::handlers::ExternalBody {
            fmt::format("Unsupported type for {} (should be: {})", Worker::table_key, "number") });

    if (not request_json[Worker::patronomic_key].IsMissing())
        if (not request_json[Worker::patronomic_key].IsString())
            throw server::handlers::ClientError(server::handlers::ExternalBody {
                fmt::format("Unsupported type for {} (should be: {})", Worker::patronomic_key, "string") });

    return { request_json[Worker::table_key].As<std::size_t>(),
        request_json[Worker::name_key].As<std::string>(),
        request_json[Worker::surname_key].As<std::string>(),
        request_json[Worker::patronomic_key].IsMissing() ? "" : request_json[Worker::patronomic_key].As<std::string>() };
}

Value AdministrationService::processWorker(Worker&& target, bool createNew) const
{
    ValueBuilder builder;
    builder["action"] = createNew ? "Creating" : "Modifying";
    ValueBuilder person;
    person["name"] = target.name;
    person["surname"] = target.surname;
    person["table_id"] = target.table_id;
    builder["who"] = person.ExtractValue();
    return builder.ExtractValue();
}

server::handlers::HttpHandlerJsonBase::Value
AdministrationService::processCreation(
    const HttpRequest&, const Value& request_json) const
{
    return processWorker(extractWorkerInfo(request_json), true);
}
server::handlers::HttpHandlerJsonBase::Value
AdministrationService::processModification(
    const HttpRequest&, const Value& request_json) const
{
    return processWorker(extractWorkerInfo(request_json), false);
}
