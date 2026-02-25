#pragma once

#include <cstddef>
#include <string>
#include <string_view>

struct Worker {
    static constexpr std::string_view table_key = "table_id";
    static constexpr std::string_view name_key = "name";
    static constexpr std::string_view surname_key = "surname";
    static constexpr std::string_view patronymic_key = "patronymic";

    std::size_t table_id;
    std::string name, surname, patronymic = {};
};
