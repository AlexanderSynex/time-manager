#include "services/details/UserService.hpp"
#include "info/Department.hpp"
#include "info/Worker.hpp"

#include <cstddef>
#include <fmt/format.h>
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

bool
UserService::isValidUser (const JsonData &request) noexcept
{
  if (not request.HasMember (Worker::Info::table_key))
    return false;
  if (not request[Worker::Info::table_key].IsInt ())
    return false;
  return true;
}

UserService::UserService (const components::ComponentContext &context,
                          std::string_view db_service_name)
    : p_db (context.FindComponent<components::Postgres> (db_service_name)
                .GetCluster ())
{
}

/// @return Дискриптор пользователя по json-запросу
std::optional<Worker>
UserService::getWorker (const JsonData &request) const
{
  if (not request[Worker::Info::table_key].IsInt ())
    {
      return {};
    }
  return Worker{ static_cast<std::size_t> (
      request[Worker::Info::table_key].As<int> ()) };
}

/// @brief Формируем запрос с данными пользователя по дискриптору
/// @details Возможно добавить кеширование
UserService::JsonData
UserService::getUserInfo (Worker &&user) const
{
  auto trx = db ()->Begin ("finding_user_info",
                           storages::postgres::ClusterHostType::kMaster, {});
  auto res = trx.Execute (worktime_postgres_service::sql::kFindUserInfoById,
                          static_cast<int> (user));
  if (not res.RowsAffected ())
    {

      throw server::handlers::InternalServerError{
        server::handlers::ExternalBody{
            "Unprocessable error while getting user info" }
      };
    }
  auto userInfo = res.Front ();
  auto userData = ValueBuilder{};
  userData[std::string{ Worker::Info::name_key }]
      = userInfo[std::string{ Worker::Info::name_key }].As<std::string> ();
  userData[std::string{ Worker::Info::surname_key }]
      = userInfo[std::string{ Worker::Info::surname_key }].As<std::string> ();
  userData[std::string{ Worker::Info::patronymic_key }]
      = userInfo[std::string{ Worker::Info::patronymic_key }]
            .As<std::string> ();
  auto data = ValueBuilder{};
  data["data"] = userData.ExtractValue ();
  return data.ExtractValue ();
}

UserService::JsonData
UserService::getUserInfo (const JsonData &request) const
{
  auto worker = getWorker (request);
  if (not worker.has_value ())
    {
      throw server::handlers::ClientError (
          server::handlers::ExternalBody{ fmt::format ("No user found") });
    }
  return getUserInfo (std::move (worker.value ()));
}

std::optional<company::Department>
UserService::getDepartment (const JsonData &request) const
{
  if (not request[company::Department::Info::id_key].IsInt ())
    {
      return {};
    }
  return company::Department{ static_cast<std::size_t> (
      request[company::Department::Info::id_key].As<int> ()) };
}

UserService::JsonData
UserService::getDepartmentInfo (const JsonData &request) const
{
  auto department = getDepartment (request);
  if (not department.has_value ())
    {
      throw server::handlers::ClientError (server::handlers::ExternalBody{
          fmt::format ("No department found") });
    }
  return getDepartmentInfo (std::move (department.value ()));
}

UserService::JsonData
UserService::getDepartmentInfo (company::Department &&depratment) const
{
  auto trx = db ()->Begin ("finding_user_info",
                           storages::postgres::ClusterHostType::kMaster, {});
  auto res
      = trx.Execute (worktime_postgres_service::sql::kFindDepartmentInfoById,
                     static_cast<int> (depratment));
  if (not res.RowsAffected ())
    {
      throw server::handlers::InternalServerError{
        server::handlers::ExternalBody{
            "Unprocessable error while getting department info" }
      };
    }
  auto user = res.Front ();
  auto userData = ValueBuilder{};
  userData[std::string{ company::Department::Info::id_key }]
      = static_cast<int> (depratment.id);
  userData[std::string{ company::Department::Info::name_key }]
      = user[std::string{ company::Department::Info::name_key }]
            .As<std::string> ();
  userData[std::string{ company::Department::Info::leader_key }]
      = user[std::string{ company::Department::Info::leader_key }].As<int> ();
  auto data = ValueBuilder{};
  data["data"] = userData.ExtractValue ();
  return data.ExtractValue ();
}
