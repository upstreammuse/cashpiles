#include "m_date.h"

#include <cassert>

using namespace model;

Date::Date(unsigned char month_, unsigned char day_, size_t year_) :
   month(month_),
   day(day_),
   year(year_)
{
   assert(month >= 1 && month <= 12);
   assert(day >= 1 && day <= daysInMonth(month, year));
}

unsigned char Date::daysInMonth(unsigned char month, size_t year)
{
   // 30 days hath september, april, june, and november
   if (month == 9 || month == 4 || month == 6 || month == 11)
   {
      return 30;
   }
   // all the rest have 31, save february alone
   else if (month != 2)
   {
      return 31;
   }
   else
   {
      // which hath 4 and 24
      if (!isLeapYear(year))
      {
         return 28;
      }
      // 'til leap year gives it one day more
      else
      {
         return 29;
      }
   }
}

bool Date::isLeapYear(size_t year)
{
   // leap years occur every 4 years, but not every 100, but every 400
   return (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;
}
