#include "services/WorktimeService.hpp"
#include "info/DBInfo.hpp"
#include "info/Worker.hpp"
#include "services/details/UserService.hpp"
#include "services/details/WorkDay.hpp"
#include "worktime_postgres_service/sql_queries.hpp"

#include <bits/chrono.h>
#include <chrono>
#include <fmt/format.h>
#include <optional>
#include <string>
#include <userver/components/component_config.hpp>
#include <userver/components/component_context.hpp>
#include <userver/formats/json/value.hpp>
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
    : details::UserService (component_context, db::info::db_name),
      handlers::HttpHandlerJsonBase (config, component_context)
{
}
handlers::HttpHandlerJsonBase::Value
WorktimeService::HandleRequestJsonThrow (const HttpRequest &request,
                                         const Value &,
                                         RequestContext &context) const
{
  constexpr auto arriveTarget = "arrive";
  constexpr auto leaveTarget = "leave";

  std::string action = request.GetPathArg ("action");
  auto handlers
      = std::unordered_map<std::string, std::function<Value (Worker &&)>>{
          { "",
            [&request, &context, this] (Worker &&user) -> Value {
              return HandleRequestInfoJsonThrow (std::move (user), request,
                                                 context);
            } },
          { arriveTarget,
            [&request, &context, this] (Worker &&user) -> Value {
              return HandleRequestArriveJsonThrow (std::move (user), request,
                                                   context);
            } },
          { leaveTarget,
            [&request, &context, this] (Worker &&user) -> Value {
              return HandleRequestLeaveJsonThrow (std::move (user), request,
                                                  context);
            } }
        };

  auto handlerIt = handlers.find (action);
  if (handlerIt == handlers.end ())
    {
      throw ClientError (
          ExternalBody{ fmt::format ("Unprocessable action: {}", action) });
    }

  auto user = getWorker (context);
  if (not user.has_value ())
    {
      throw server::handlers::ClientError (
          server::handlers::ExternalBody{ "No user table_id provided" });
    }
  return handlerIt->second (std::move (user.value ()));
}

handlers::HttpHandlerJsonBase::Value
WorktimeService::HandleRequestArriveJsonThrow (Worker &&user,
                                               const HttpRequest &request,
                                               RequestContext &) const
{
  switch (request.GetMethod ())
    {
    case userver::v2_15::server::http::HttpMethod::kPut:
      {
        auto arriveTime = workerArrived (std::move (user));
        if (not arriveTime.has_value ())
          {
            throw server::handlers::ClientError (
                server::handlers::ExternalBody{
                    "Problem with commiting arrive-worktime" });
          }

        return prepareMessage (
            std::move (arriveTime.value ()),
            isOnWork (user, std::move (arriveTime.value ())));
      }
    default:
      throw server::handlers::ClientError (server::handlers::ExternalBody{
          fmt::format ("Unsupported method {}", request.GetMethod ()) });
    }
}

handlers::HttpHandlerJsonBase::Value
WorktimeService::HandleRequestLeaveJsonThrow (Worker &&user,
                                              const HttpRequest &request,
                                              RequestContext &) const
{
  switch (request.GetMethod ())
    {
    case userver::v2_15::server::http::HttpMethod::kPut:
      {
        auto leaveTime = workerLeaved (user);
        return prepareMessage (
            std::move (leaveTime),
            isOnWork (user, std::move (leaveTime.value ())));
      }
    default:
      throw server::handlers::ClientError (server::handlers::ExternalBody{
          fmt::format ("Unsupported method {}", request.GetMethod ()) });
    }
}

userver::server::handlers::HttpHandlerJsonBase::Value
services::control_role::WorktimeService::HandleRequestInfoJsonThrow (
    Worker &&user, const HttpRequest &request, RequestContext &) const
{
  if (request.GetMethod () != userver::v2_15::server::http::HttpMethod::kGet)
    {
      throw server::handlers::ClientError (server::handlers::ExternalBody{
          fmt::format ("Unsupported method {}", request.GetMethod ()) });
    }

  auto builder = formats::json::ValueBuilder{};
  builder["table_id"] = std::to_string (user.id);
  const auto worked = isWorked (user);
  if (not worked)
    {
      return builder.ExtractValue ();
    }
  builder["time"]
      = [] (std::optional<storages::postgres::TimePointTz> &&arrived,
            std::optional<storages::postgres::TimePointTz> &&departed,
            bool isOnWork) -> formats::json::Value {
    auto builder = formats::json::ValueBuilder{};
    builder["on_work"] = isOnWork;
    if (arrived.has_value ())
      builder["arrived"] = arrived.value ();
    if (departed.has_value ())
      builder["departed"] = departed.value ();
    return builder.ExtractValue ();
  }(getArrivalTime (user), getLeftTime (user), isOnWork (user));
  return builder.ExtractValue ();
}

std::optional<userver::storages::postgres::TimePointTz>
WorktimeService::workerArrived (const Worker &user) const
{
  auto trx = db ()->Begin ("worker_arrived_transaction",
                           storages::postgres::ClusterHostType::kMaster, {});
  auto res = trx.Execute (worktime_postgres_service::sql::kArriveWorker,
                          static_cast<int> (user));
  if (res.RowsAffected ())
    trx.Commit ();
  else
    trx.Rollback ();
  return getArrivalTime (std::move (user));
}

std::optional<userver::storages::postgres::TimePointTz>
WorktimeService::workerLeaved (const Worker &user) const
{
  auto trx = db ()->Begin ("worker_left_transaction",
                           storages::postgres::ClusterHostType::kMaster, {});
  auto res = trx.Execute (worktime_postgres_service::sql::kDepartWorker,
                          static_cast<int> (user));
  if (res.RowsAffected ())
    trx.Commit ();
  else
    trx.Rollback ();
  return getLeftTime (std::move (user));
}

std::optional<userver::storages::postgres::TimePointTz>
services::control_role::WorktimeService::getUserWorktimeFromDB (
    const userver::storages::Query &query, const Worker &user,
    utils::time::WorkDay &&when) const
{
  using namespace std::chrono;
  auto trx = db ()->Begin ("get_worker_time_transaction",
                           storages::postgres::ClusterHostType::kMaster, {});

  auto res = trx.Execute (query, static_cast<int> (user),
                          userver::utils::datetime::ToString (when ()));
  trx.Rollback ();
  auto result
      = res.AsSingleRow<std::optional<storages::postgres::TimePointTz>> ();
  return result;
}

std::optional<userver::storages::postgres::TimePointTz>
services::control_role::WorktimeService::getArrivalTime (
    const Worker &user, utils::time::WorkDay &&when) const
{
  return getUserWorktimeFromDB (
      worktime_postgres_service::sql::kGetArrivalTime, user, std::move (when));
}

std::optional<userver::storages::postgres::TimePointTz>
services::control_role::WorktimeService::getLeftTime (
    const Worker &user, utils::time::WorkDay &&when) const
{
  return getUserWorktimeFromDB (worktime_postgres_service::sql::kGetDepartTime,
                                user, std::move (when));
}

bool
services::control_role::WorktimeService::isOnWork (
    const Worker &user, std::chrono::system_clock::time_point &&when) const
{
  auto arrived = getArrivalTime (Worker (user),
                                 utils::time::WorkDay{ std::move (when) });
  auto departed
      = getLeftTime (Worker (user), utils::time::WorkDay{ std::move (when) });
  return arrived.has_value () and not departed.has_value ();
}

userver::server::handlers::HttpHandlerJsonBase::Value
services::control_role::WorktimeService::prepareMessage (
    std::optional<userver::storages::postgres::TimePointTz> &&tp,
    bool isOnWork) const
{
  auto b = userver::formats::json::ValueBuilder{};
  if (tp.has_value ())
    {
      b["when"] = tp.value ();
    }
  b["on_work"] = isOnWork;
  return b.ExtractValue ();
}

bool
services::control_role::WorktimeService::isWorked (
    const Worker &user, utils::time::WorkDay &&when) const
{
  auto trx = db ()->Begin ("is-user-worked-transaction",
                           storages::postgres::ClusterHostType::kMaster, {});
  auto res = trx.Execute (worktime_postgres_service::sql::kCheckUserWorked,
                          static_cast<int> (user),
                          userver::utils::datetime::ToString (when ()));
  trx.Rollback ();
  return res.AsSingleRow<int> () == 1;
}
