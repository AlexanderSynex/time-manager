#include "services/details/UserService.hpp"
#include "info/worker.hpp"

#include <fmt/format.h>
#include <optional>
#include <string_view>
#include <userver/formats/json/value_builder.hpp>
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

Worker UserService::extract(const JsonData& request) const
{
    if (not request[Worker::name_key].IsString())
        throw server::handlers::ClientError(server::handlers::ExternalBody {
            fmt::format("Unsupported type for {} (should be: {})", Worker::name_key, "string") });

    if (not request[Worker::surname_key].IsString())
        throw server::handlers::ClientError(server::handlers::ExternalBody {
            fmt::format("Unsupported type for {} (should be: {})", Worker::surname_key, "string") });

    if (not request[Worker::table_key].IsNumber())
        throw server::handlers::ClientError(server::handlers::ExternalBody {
            fmt::format("Unsupported type for {} (should be: {})", Worker::table_key, "number") });

    if (not request.HasMember(Worker::patronymic_key))
        if (not request[Worker::patronymic_key].IsString() and not request[Worker::patronymic_key].IsMissing())
            throw server::handlers::ClientError(server::handlers::ExternalBody {
                fmt::format("Unsupported type for {} (should be: {})", Worker::patronymic_key, "string") });

    return Worker { static_cast<std::size_t>(request[Worker::table_key].As<int>()),
        request[Worker::name_key].As<std::string>(),
        request[Worker::surname_key].As<std::string>(),
        [request]() -> std::string {
            if (not request.HasMember(Worker::patronymic_key)) {
                return "";
            }
            return request[Worker::patronymic_key].As<std::string>();
        }() };
}

bool UserService::validate(const JsonData& request) const noexcept
{
    if (not request.HasMember(Worker::table_key) or not request.HasMember(Worker::name_key) or not request.HasMember(Worker::surname_key))
        return false;
    if (not request[Worker::table_key].IsInt())
        return false;
    if (not request[Worker::name_key].IsString())
        return false;
    if (not request[Worker::surname_key].IsString())
        return false;

    if (request.HasMember(Worker::patronymic_key)) {
        if (not request[Worker::patronymic_key].IsString()) {
            return false;
        }
    }
    return true;
}

UserService::JsonData UserService::find(std::size_t table_id) const
{
    auto trx = db()->Begin("finding_user_by_table_id", storages::postgres::ClusterHostType::kMaster, {});
    auto res = trx.Execute(worktime_postgres_service::sql::kFindUserByTableId, static_cast<int>(table_id));
    if (res.RowsAffected()) {
        auto user = res.Front();
        auto userData = ValueBuilder {};
        userData[std::string { Worker::name_key }] = user[std::string { Worker::name_key }].As<std::string>();
        userData[std::string { Worker::surname_key }] = user[std::string { Worker::surname_key }].As<std::string>();
        userData[std::string { Worker::patronymic_key }] = user[std::string { Worker::patronymic_key }].As<std::string>();
        auto data = ValueBuilder {};
        data["data"] = userData.ExtractValue();
        return data.ExtractValue();
    }

    throw server::handlers::ClientError(server::handlers::ExternalBody { fmt::format("No user with table_id: {}", table_id) });
}
