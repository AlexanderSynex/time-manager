#pragma once

#include <cstddef>
#include <string>

struct Worker {
  std::size_t table_id;
  std::string name, surname, patronomic = {};
};
