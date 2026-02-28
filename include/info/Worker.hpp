#pragma once

#include <cstddef>
#include <string>
#include <string_view>
#include <userver/formats/json/value.hpp>

struct Worker
{
  struct Info
  {
    static constexpr std::string_view table_key = "table_id";
    static constexpr std::string_view name_key = "name";
    static constexpr std::string_view surname_key = "surname";
    static constexpr std::string_view patronymic_key = "patronymic";

    std::size_t table_id;
    std::string name, surname, patronymic = {};
  };

  static Info extractInfo (const userver::formats::json::Value &);

  operator int () const { return static_cast<int> (id); }
  std::size_t id;
};