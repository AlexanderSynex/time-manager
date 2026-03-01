#pragma once

#include "info/Worker.hpp"
#include "services/details/UserService.hpp"

#include <optional>
#include <string_view>
#include <userver/clients/dns/component.hpp>
#include <userver/components/component_config.hpp>
#include <userver/components/component_context.hpp>
#include <userver/components/component_list.hpp>
#include <userver/server/handlers/http_handler_json_base.hpp>

#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/storages/postgres/postgres_fwd.hpp>
#include <userver/testsuite/testsuite_support.hpp>

namespace services::control_role
{

///@brief Служебный сервис для управления данными о пользователях
class AdministrationService final
    : public details::UserService,
      public userver::server::handlers::HttpHandlerJsonBase
{
  static constexpr auto userTarget = "user";
  static constexpr auto departmentTarget = "department";

public:
  static constexpr std::string_view kName = "administration-service";

  explicit AdministrationService (
      const userver::components::ComponentConfig &config,
      const userver::components::ComponentContext &component_context);

  Value HandleRequestJsonThrow (const HttpRequest &request,
                                const Value &request_json,
                                RequestContext &context) const override;

private:
  Value HandleUserJsonThrow (const HttpRequest &request,
                             const Value &request_json,
                             RequestContext &context) const;

  Value HandleDepartmentJsonThrow (const HttpRequest &request,
                                   const Value &request_json,
                                   RequestContext &context) const;

  bool modifyUserInfo (const Value &request_json) const;
  bool modifyUserInfo (Worker &&user, Worker::Info &&info) const;

private:
  userver::storages::postgres::ClusterPtr p_db = nullptr;
};

} // namespace services::control_role
