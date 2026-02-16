#include "add_user.hpp"

#include <fmt/format.h>

std::string add_user(Worker &&user) {
  return fmt::format("INSERT INTO users(table_id, name, surname, patronomic) "
                     "VALUES ({}, {}, {}, {})",
                     user.table_id, user.name, user.surname, user.patronomic);
}
