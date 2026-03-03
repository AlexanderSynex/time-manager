#pragma once

#include "info/Worker.hpp"
#include "services/details/UserService.hpp"
#include <optional>
#include <userver/clients/dns/component.hpp>
#include <userver/components/component_list.hpp>
#include <userver/formats/json/value_builder.hpp>
#include <userver/server/handlers/http_handler_json_base.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>

#include <string_view>
#include <userver/storages/postgres/postgres_fwd.hpp>
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

  Value HandleRequestArriveJsonThrow (const HttpRequest &request,
                                      const Value &request_json,
                                      RequestContext &) const;

  Value HandleRequestLeaveJsonThrow (const HttpRequest &request,
                                     const Value &request_json,
                                     RequestContext &) const;

private:
  std::optional<userver::storages::postgres::TimePointTz>
  getArrivalTime (Worker &&user,
                  std::chrono::system_clock::time_point &&when) const;

  std::optional<userver::storages::postgres::TimePointTz>
  getLeftTime (Worker &&user,
               std::chrono::system_clock::time_point &&when) const;

  std::optional<userver::storages::postgres::TimePointTz>
  workerArrived (Worker &&info) const;
  std::optional<userver::storages::postgres::TimePointTz>
  workerLeaved (Worker &&info) const;

  Value
  prepareMessage (userver::storages::postgres::TimePointTz &&tp) const
  {
    auto b = userver::formats::json::ValueBuilder{};
    b["when"] = tp;
    return b.ExtractValue ();
  }

private:
  userver::storages::postgres::ClusterPtr p_db = nullptr;
};
}
