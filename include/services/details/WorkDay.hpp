#pragma once

#include <bits/chrono.h>
#include <chrono>
#include <userver/utils/datetime/date.hpp>

namespace services::utils::time
{
struct WorkDay : std::chrono::year_month_day
{
  explicit WorkDay (std::chrono::system_clock::time_point &&time
                    = std::chrono::system_clock::now ())
      : std::chrono::year_month_day (
            std::chrono::floor<std::chrono::days> (time))
  {
  }

  userver::utils::datetime::Date
  operator() () const
  {
    return userver::utils::datetime::Date (static_cast<int> (year ()),
                                           static_cast<unsigned> (month ()),
                                           static_cast<unsigned> (day ()));
  }

  static utils::time::WorkDay
  today ()
  {
    return WorkDay (std::chrono::system_clock::now ());
  }
};

}