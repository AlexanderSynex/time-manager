

#include "info/Worker.hpp"
#include <fmt/format.h>
#include <userver/server/handlers/exceptions.hpp>

Worker::Info
Worker::extractInfo (const userver::formats::json::Value &request_json)
{
  if (not request_json.HasMember (Worker::Info::table_key))
    {
      throw userver::server::handlers::ClientError (
          userver::server::handlers::ExternalBody{
              fmt::format ("No '{}' value provided", "table_id") });
    }
  if (not request_json[Worker::Info::table_key].IsInt ())
    {
      throw userver::server::handlers::ClientError (
          userver::server::handlers::ExternalBody{ fmt::format (
              "Value '{}' should be '{}': got '{}'", "table_id", "integer",
              request_json[Worker::Info::table_key].As<std::string> ()) });
    }

  if (not request_json.HasMember (Worker::Info::name_key))
    {
      throw userver::server::handlers::ClientError (
          userver::server::handlers::ExternalBody{
              fmt::format ("No '{}' value provided", "name") });
    }
  if (not request_json[Worker::Info::name_key].IsString ())
    {
      throw userver::server::handlers::ClientError (
          userver::server::handlers::ExternalBody{ fmt::format (
              "Value '{}' should be '{}': got '{}'", "name", "string",
              request_json[Worker::Info::name_key].As<std::string> ()) });
    }

  if (not request_json.HasMember (Worker::Info::surname_key))
    {
      throw userver::server::handlers::ClientError (
          userver::server::handlers::ExternalBody{
              fmt::format ("No '{}' value provided", "surname") });
    }
  if (not request_json[Worker::Info::surname_key].IsString ())
    {
      throw userver::server::handlers::ClientError (
          userver::server::handlers::ExternalBody{ fmt::format (
              "Value '{}' should be '{}': got '{}'", "surname", "string",
              request_json[Worker::Info::surname_key].As<std::string> ()) });
    }

  if (request_json.HasMember (Worker::Info::patronymic_key))
    {
      if (not request_json[Worker::Info::patronymic_key].IsString ())
        {
          throw userver::server::handlers::ClientError (
              userver::server::handlers::ExternalBody{
                  fmt::format ("Value '{}' should be '{}': got '{}'",
                               "patronymic", "string",
                               request_json[Worker::Info::patronymic_key]
                                   .As<std::string> ()) });
        }
    }

  return Worker::Info{
    static_cast<std::size_t> (
        request_json[Worker::Info::table_key].As<int> ()),
    request_json[Worker::Info::name_key].As<std::string> (),
    request_json[Worker::Info::surname_key].As<std::string> (),
    request_json[Worker::Info::patronymic_key].As<std::string> ({})
  };
}