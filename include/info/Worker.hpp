#pragma once

#include <cstddef>
#include <optional>
#include <string>
#include <string_view>
#include <userver/formats/json/value.hpp>

struct Worker
{
  static constexpr std::string_view table_key = "table_id";
  struct Info
  {
    static constexpr std::string_view name_key = "name";
    static constexpr std::string_view surname_key = "surname";
    static constexpr std::string_view patronymic_key = "patronymic";
    static constexpr std::string_view password_key = "password";
    std::optional<std::string> password = {}, name = {}, surname = {},
                               patronymic = {};
  };
  static Info extractInfo (const userver::formats::json::Value &);
  operator int () const { return static_cast<int> (id); }
  std::size_t id;
};