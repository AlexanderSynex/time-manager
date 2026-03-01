#include "services/details/UserService.hpp"
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
UserService::isValid (const JsonData &request) noexcept
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
  if (res.RowsAffected ())
    {
      auto user = res.Front ();
      auto userData = ValueBuilder{};
      userData[std::string{ Worker::Info::name_key }]
          = user[std::string{ Worker::Info::name_key }].As<std::string> ();
      userData[std::string{ Worker::Info::surname_key }]
          = user[std::string{ Worker::Info::surname_key }].As<std::string> ();
      userData[std::string{ Worker::Info::patronymic_key }]
          = user[std::string{ Worker::Info::patronymic_key }]
                .As<std::string> ();
      auto data = ValueBuilder{};
      data["data"] = userData.ExtractValue ();
      return data.ExtractValue ();
    }
  throw server::handlers::InternalServerError{ server::handlers::ExternalBody{
      "Unprocessable error while getting user info" } };
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