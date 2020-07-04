#pragma once

#include <cstddef>

namespace model { struct Interval; }

struct model::Interval
{
   enum class Period
   {
      DAYS,
      MONTHS,
      YEARS
   };
   size_t number;
   Period period;

   Interval(size_t, Period);
};
