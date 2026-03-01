#include "services/AdministrationService.hpp"
#include "info/Worker.hpp"
#include "services/details/UserService.hpp"

#include <fmt/format.h>
#include <functional>
#include <map>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <userver/components/component_config.hpp>
#include <userver/components/component_context.hpp>
#include <userver/formats/json/exception.hpp>
#include <userver/formats/json/value.hpp>
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

AdministrationService::AdministrationService (
    const components::ComponentConfig &config,
    const components::ComponentContext &component_context)
    : details::UserService (component_context, "db"),
      userver::server::handlers::HttpHandlerJsonBase::HttpHandlerJsonBase (
          config, component_context)
{
}

Value
AdministrationService::HandleRequestJsonThrow (const HttpRequest &request,
                                               const Value &request_json,
                                               RequestContext &context) const
{
  std::string target = request.GetPathArg ("target");
  auto handlers = std::unordered_map<std::string, std::function<Value ()>>{
    { userTarget,
      [&request, &request_json, &context, this] () -> Value {
        return HandleUserJsonThrow (request, request_json, context);
      } },
    { departmentTarget,
      [&request, &request_json, &context, this] () -> Value {
        return HandleDepartmentJsonThrow (request, request_json, context);
      } }
  };

  auto handlerIt = handlers.find (target);
  if (handlerIt == handlers.end ())
    {
      throw ClientError (
          ExternalBody{ fmt::format ("Unprocessable target: {}", target) });
    }

  return handlerIt->second ();
}

Value
AdministrationService::HandleUserJsonThrow (const HttpRequest &request,
                                            const Value &request_json,
                                            RequestContext &) const
{
  switch (request.GetMethod ())
    {
    case server::http::HttpMethod::kPut:
      {
        if (modifyUserInfo (request_json))
          {
            request.GetHttpResponse ().SetStatus (
                userver::v2_15::http::kCreated);
          }
        return getUserInfo (request_json);
      }
    default:
      throw server::handlers::ClientError (server::handlers::ExternalBody{
          fmt::format ("Unsupported method {}", request.GetMethod ()) });
    }
}

Value
AdministrationService::HandleDepartmentJsonThrow (const HttpRequest &request,
                                                  const Value &request_json,
                                                  RequestContext &) const
{
  switch (request.GetMethod ())
    {
    case server::http::HttpMethod::kPut:
      {
        if (modifyUserInfo (request_json))
          {
            request.GetHttpResponse ().SetStatus (
                userver::v2_15::http::kCreated);
          }
        return getUserInfo (request_json);
      }
    default:
      throw server::handlers::ClientError (server::handlers::ExternalBody{
          fmt::format ("Unsupported method {}", request.GetMethod ()) });
    }
}

bool
AdministrationService::modifyUserInfo (const Value &request_json) const
{
  auto user = getWorker (request_json);

  if (not user.has_value ())
    {
      throw ClientError (ExternalBody{ "No table_id provided" });
    }

  return modifyUserInfo (std::move (user.value ()),
                         Worker::extractInfo (request_json));
}

bool
AdministrationService::modifyUserInfo (Worker &&user,
                                       Worker::Info &&info) const
{
  auto trx = db ()->Begin ("managing_user_transaction",
                           storages::postgres::ClusterHostType::kMaster, {});
  auto res = trx.Execute (worktime_postgres_service::sql::kUpdateUser,
                          static_cast<int> (user), info.name, info.surname,
                          info.patronymic);
  if (res.RowsAffected ())
    {
      trx.Commit ();
      return true;
    }
  trx.Rollback ();
  return false;
}