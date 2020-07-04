#pragma once

#include <cstddef>

namespace model { struct Date; }

struct model::Date
{
   unsigned char month;
   unsigned char day;
   size_t year;

   Date(unsigned char month, unsigned char day, size_t year);
   static unsigned char daysInMonth(unsigned char month, size_t year);
   static bool isLeapYear(size_t year);
};
