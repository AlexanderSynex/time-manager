#include "services/AdministrationService.hpp"
#include "info/worker.hpp"
#include "services/details/UserService.hpp"

#include <fmt/format.h>
#include <userver/components/component_config.hpp>
#include <userver/components/component_context.hpp>
#include <userver/formats/json/exception.hpp>
#include <userver/http/status_code.hpp>
#include <userver/logging/log.hpp>
#include <userver/server/handlers/exceptions.hpp>
#include <userver/storages/postgres/cluster_types.hpp>

#include <userver/formats/json/value_builder.hpp>
#include <userver/server/handlers/http_handler_json_base.hpp>
#include <userver/server/http/http_method.hpp>
#include <userver/storages/postgres/cluster.hpp>

#include <worktime_postgres_service/sql_queries.hpp>

using namespace userver;
using namespace userver::formats::json;
using namespace services::control_role;

AdministrationService::AdministrationService(const components::ComponentConfig& config,
    const components::ComponentContext& component_context)
    : details::UserService(component_context, "db")
    , userver::server::handlers::HttpHandlerJsonBase::HttpHandlerJsonBase(config, component_context)
{
}

Value AdministrationService::HandleRequestJsonThrow(const HttpRequest& request,
    const Value& request_json,
    RequestContext&) const
{
    switch (request.GetMethod()) {
    case server::http::HttpMethod::kPut:
        return processWorker(extract(request_json), request);
    default:
        throw server::handlers::ClientError(server::handlers::ExternalBody {
            fmt::format("Unsupported method {}", request.GetMethod()) });
    }
}

Value AdministrationService::processWorker(Worker&& user, const HttpRequest& request) const
{
    auto trx = db()->Begin("managing_user_transaction", storages::postgres::ClusterHostType::kMaster, {});
    auto res = trx.Execute(worktime_postgres_service::sql::kUpdateUser, static_cast<int>(user.table_id), user.name, user.surname, user.patronymic);
    if (res.RowsAffected()) {
        trx.Commit();
        request.GetHttpResponse().SetStatus(userver::v2_15::http::kCreated);
        return this->find(user.table_id);
    }
    trx.Rollback();
    throw server::handlers::ClientError(server::handlers::ExternalBody {
        fmt::format("Unprocessable worker: {} {} {} {}", user.table_id, user.name, user.surname, user.patronymic) });
}
