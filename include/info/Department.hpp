#pragma once

#include <cstddef>
#include <optional>
#include <string>
#include <string_view>
#include <userver/formats/json/value.hpp>

namespace company
{

struct Department
{
  struct Info
  {
    static constexpr std::string_view id_key = "id";
    static constexpr std::string_view name_key = "name";
    static constexpr std::string_view leader_key = "leader_id";
    static constexpr std::string_view internal_number_key = "internal_id";

    std::string name;
    std::size_t leader_id;
    std::optional<std::size_t> internal_id = {};
  };

  static Info extractInfo (const userver::formats::json::Value &);

  operator int () const { return static_cast<int> (id); }
  std::size_t id;
};

}