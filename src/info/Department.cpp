#include "info/Department.hpp"
#include <cstddef>
#include <optional>
#include <userver/formats/json/value.hpp>
#include <userver/server/handlers/exceptions.hpp>

using namespace company;

Department::Info
Department::extractInfo (const userver::formats::json::Value &request_json)
{
  if (not request_json.HasMember (Info::name_key))
    {
      throw userver::server::handlers::ClientError (
          userver::server::handlers::ExternalBody{
              fmt::format ("No '{}' value provided", Info::name_key) });
    }
  if (not request_json[Info::name_key].IsString ())
    {
      throw userver::server::handlers::ClientError (
          userver::server::handlers::ExternalBody{ fmt::format (
              "Value '{}' should be '{}': got '{}'", Info::name_key, "string",
              request_json[Info::name_key].As<std::string> ()) });
    }

  if (not request_json.HasMember (Info::leader_key))
    {
      throw userver::server::handlers::ClientError (
          userver::server::handlers::ExternalBody{
              fmt::format ("No '{}' value provided", Info::leader_key) });
    }
  if (not request_json[Info::leader_key].IsInt ())
    {
      throw userver::server::handlers::ClientError (
          userver::server::handlers::ExternalBody{ fmt::format (
              "Value '{}' should be '{}': got '{}'", Info::leader_key,
              "integer", request_json[Info::leader_key].As<std::string> ()) });
    }

  if (not request_json[Info::internal_number_key].IsInt ()
      and not request_json[Info::internal_number_key].IsMissing ())
    {
      throw userver::server::handlers::ClientError (
          userver::server::handlers::ExternalBody{ fmt::format (
              "Value '{}' should be '{}': got '{}'", Info::internal_number_key,
              "integer",
              request_json[Info::internal_number_key].As<std::string> ()) });
    }

  return Department::Info{ request_json[Info::name_key].As<std::string> (),
                           static_cast<std::size_t> (
                               request_json[Info::leader_key].As<int> ()) };
}
