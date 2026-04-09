#pragma once

#include "info/Worker.hpp"
#include "services/details/UserService.hpp"
#include "services/details/WorkDay.hpp"
#include <chrono>
#include <optional>
#include <userver/clients/dns/component.hpp>
#include <userver/components/component_list.hpp>
#include <userver/formats/json/value_builder.hpp>
#include <userver/server/handlers/http_handler_json_base.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>

#include <string_view>
#include <userver/storages/postgres/postgres_fwd.hpp>
#include <userver/storages/query.hpp>
#include <userver/testsuite/testsuite_support.hpp>

namespace services::control_role
{

///@brief Пользовательский сервис для учета рабочего времени пользователей
class WorktimeService final
    : public details::UserService,
      public userver::server::handlers::HttpHandlerJsonBase
{
public:
  static constexpr std::string_view kName = "worktime-service";

  explicit WorktimeService (
      const userver::components::ComponentConfig &config,
      const userver::components::ComponentContext &component_context);

  Value HandleRequestJsonThrow (const HttpRequest &request,
                                const Value &request_json,
                                RequestContext &) const override;

  Value HandleRequestInfoJsonThrow (Worker &&user, const HttpRequest &request,
                                    RequestContext &) const;

  Value HandleRequestArriveJsonThrow (Worker &&user,
                                      const HttpRequest &request,
                                      RequestContext &) const;

  Value HandleRequestLeaveJsonThrow (Worker &&user, const HttpRequest &request,
                                     RequestContext &) const;

private:
  std::optional<userver::storages::postgres::TimePointTz>
  getUserWorktimeFromDB (const userver::storages::Query &, const Worker &user,
                         utils::time::WorkDay &&when
                         = utils::time::WorkDay::today ()) const;

  std::optional<userver::storages::postgres::TimePointTz>
  getArrivalTime (const Worker &user, utils::time::WorkDay &&when
                                      = utils::time::WorkDay::today ()) const;

  std::optional<userver::storages::postgres::TimePointTz>
  getLeftTime (const Worker &user, utils::time::WorkDay &&date
                                   = utils::time::WorkDay::today ()) const;

  std::optional<userver::storages::postgres::TimePointTz>
  workerArrived (const Worker &user) const;

  std::optional<userver::storages::postgres::TimePointTz>
  workerLeaved (const Worker &user) const;

  bool isOnWork (const Worker &user,
                 std::chrono::system_clock::time_point &&when
                 = std::chrono::system_clock::now ()) const;

  bool isWorked (const Worker &user, utils::time::WorkDay &&when
                                     = utils::time::WorkDay::today ()) const;
  Value
  prepareMessage (std::optional<userver::storages::postgres::TimePointTz> &&tp,
                  bool isOnWork) const;
};
}
