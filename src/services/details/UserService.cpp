#include "services/details/UserService.hpp"
#include "info/worker.hpp"
#include <cassert>
#include <fmt/format.h>
#include <string_view>
#include <userver/logging/log.hpp>
#include <userver/server/handlers/exceptions.hpp>
#include <userver/server/server.hpp>
#include <userver/storages/postgres/cluster_types.hpp>
#include <userver/storages/postgres/component.hpp>

#include <worktime_postgres_service/sql_queries.hpp>

using namespace userver;
namespace pg = userver::storages::postgres;
using namespace userver::formats::json;
using namespace services::control_role::details;

UserService::UserService(const components::ComponentContext& context, std::string_view db_service_name)
    : p_db(context.FindComponent<components::Postgres>(db_service_name).GetCluster())
{
}

Worker UserService::extract(const Value& request_json) const
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

    // if (not request_json.HasMember(Worker::patronomic_key))
    //     if (not request_json[Worker::patronomic_key].IsString() and not request_json[Worker::patronomic_key].IsMissing())
    //         throw server::handlers::ClientError(server::handlers::ExternalBody {
    //             fmt::format("Unsupported type for {} (should be: {})", Worker::patronomic_key, "string") });

    return Worker { request_json[Worker::table_key].As<std::size_t>(),
        request_json[Worker::name_key].As<std::string>(),
        request_json[Worker::surname_key].As<std::string>(),
        "" };
}

bool UserService::validate(const Value& request_json) const noexcept
{
    if (not request_json.HasMember(Worker::table_key) or not request_json.HasMember(Worker::name_key) or not request_json.HasMember(Worker::surname_key))
        return false;
    if (not request_json[Worker::table_key].IsInt())
        return false;
    if (not request_json[Worker::name_key].IsString())
        return false;
    if (not request_json[Worker::surname_key].IsString())
        return false;

    if (request_json.HasMember(Worker::patronomic_key)) {
        if (not request_json[Worker::patronomic_key].IsString()) {
            return false;
        }
    }
    return true;
}
