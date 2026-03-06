#include "services/WorktimeService.hpp"
#include "services/details/UserService.hpp"
#include "worktime_postgres_service/sql_queries.hpp"

#include <bits/chrono.h>
#include <chrono>
#include <fmt/format.h>
#include <string>
#include <userver/components/component_config.hpp>
#include <userver/components/component_context.hpp>
#include <userver/formats/json/value_builder.hpp>
#include <userver/formats/yaml/value.hpp>
#include <userver/logging/log.hpp>
#include <userver/server/handlers/exceptions.hpp>
#include <userver/server/handlers/http_handler_json_base.hpp>
#include <userver/server/http/http_method.hpp>
#include <userver/storages/postgres/cluster_types.hpp>
#include <userver/storages/postgres/io/chrono.hpp>
#include <userver/utils/datetime.hpp>
#include <userver/utils/datetime/date.hpp>

using namespace userver;
using namespace userver::server;
using namespace services::control_role;

WorktimeService::WorktimeService (
    const components::ComponentConfig &config,
    const components::ComponentContext &component_context)
    : details::UserService (component_context, "db"),
      handlers::HttpHandlerJsonBase (config, component_context)
{
}
handlers::HttpHandlerJsonBase::Value
WorktimeService::HandleRequestJsonThrow (const HttpRequest &request,
                                         const Value &request_json,
                                         RequestContext &context) const
{
  constexpr auto arriveTarget = "arrive";
  constexpr auto leaveTarget = "leave";

  std::string action = request.GetPathArg ("action");
  auto handlers = std::unordered_map<std::string, std::function<Value ()>>{
    { arriveTarget,
      [&request, &request_json, &context, this] () -> Value {
        return HandleRequestArriveJsonThrow (request, request_json, context);
      } },
    { leaveTarget,
      [&request, &request_json, &context, this] () -> Value {
        return HandleRequestLeaveJsonThrow (request, request_json, context);
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

handlers::HttpHandlerJsonBase::Value
WorktimeService::HandleRequestArriveJsonThrow (const HttpRequest &request,
                                               const Value &request_json,
                                               RequestContext &) const
{
  switch (request.GetMethod ())
    {
    case userver::v2_15::server::http::HttpMethod::kPut:
      {
        if (not isValidUser (request_json))
          {
            throw server::handlers::ClientError (
                server::handlers::ExternalBody{ "No user table_id provided" });
          }
        auto arriveTime
            = workerArrived (std::move (getWorker (request_json).value ()));
        if (not arriveTime.has_value ())
          {
            throw server::handlers::ClientError (
                server::handlers::ExternalBody{
                    "Problem with commiting arrive-worktime" });
          }

        return prepareMessage (std::move (arriveTime.value ()), true);
      }
    default:
      throw server::handlers::ClientError (server::handlers::ExternalBody{
          fmt::format ("Unsupported method {}", request.GetMethod ()) });
    }
}

handlers::HttpHandlerJsonBase::Value
WorktimeService::HandleRequestLeaveJsonThrow (const HttpRequest &request,
                                              const Value &request_json,
                                              RequestContext &) const
{
  switch (request.GetMethod ())
    {
    case userver::v2_15::server::http::HttpMethod::kPut:
      {
        if (not isValidUser (request_json))
          {
            throw server::handlers::ClientError (
                server::handlers::ExternalBody{ "Unknown user provided: {}" });
          }
        auto leaveTime
            = workerLeaved (std::move (getWorker (request_json).value ()));
        return prepareMessage (std::move (leaveTime), false);
      }
    default:
      throw server::handlers::ClientError (server::handlers::ExternalBody{
          fmt::format ("Unsupported method {}", request.GetMethod ()) });
    }
}

std::optional<userver::storages::postgres::TimePointTz>
WorktimeService::workerArrived (Worker &&user) const
{
  auto trx = db ()->Begin ("worker_arrived_transaction",
                           storages::postgres::ClusterHostType::kMaster, {});
  auto res = trx.Execute (worktime_postgres_service::sql::kArriveWorker,
                          static_cast<int> (user));
  if (res.RowsAffected ())
    trx.Commit ();
  else
    trx.Rollback ();
  return getArrivalTime (std::move (user), std::chrono::system_clock::now ());
}

std::optional<userver::storages::postgres::TimePointTz>
WorktimeService::workerLeaved (Worker &&user) const
{
  auto trx = db ()->Begin ("worker_left_transaction",
                           storages::postgres::ClusterHostType::kMaster, {});
  auto res = trx.Execute (worktime_postgres_service::sql::kDepartWorker,
                          static_cast<int> (user));
  if (res.RowsAffected ())
    trx.Commit ();
  else
    trx.Rollback ();
  return getLeftTime (std::move (user), std::chrono::system_clock::now ());
}

std::optional<userver::storages::postgres::TimePointTz>
services::control_role::WorktimeService::getTimeFromDb (
    const userver::storages::Query &query, std::string_view transactionName,
    Worker &&user, std::chrono::system_clock::time_point &&when) const
{
  auto trx = db ()->Begin (std::string{ transactionName },
                           storages::postgres::ClusterHostType::kMaster, {});
  auto date = [time = std::move (when)] () {
    auto date = std::chrono::year_month_day{
      std::chrono::floor<std::chrono::days> (time)
    };
    return userver::utils::datetime::Date (
        static_cast<int> (date.year ()), static_cast<unsigned> (date.month ()),
        static_cast<unsigned> (date.day ()));
  }();

  auto res = trx.Execute (query, static_cast<int> (user),
                          utils::datetime::ToString (date));
  trx.Rollback ();
  return res.AsOptionalSingleRow<storages::postgres::TimePointTz> ();
}

std::optional<userver::storages::postgres::TimePointTz>
services::control_role::WorktimeService::getArrivalTime (
    Worker &&user, std::chrono::system_clock::time_point &&when) const
{
  return getTimeFromDb (worktime_postgres_service::sql::kGetArrivalTime,
                        "get_worker_arrive_time_transaction", std::move (user),
                        std::move (when));
}

std::optional<userver::storages::postgres::TimePointTz>
services::control_role::WorktimeService::getLeftTime (
    Worker &&user, std::chrono::system_clock::time_point &&when) const
{
  return getTimeFromDb (worktime_postgres_service::sql::kGetDepartTime,
                        "get_worker_leave_time_transaction", std::move (user),
                        std::move (when));
}
