#include "m_date.h"

#include <cassert>
#include <stdexcept>

using namespace util;

using std::string;

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

Date Date::parseDate(string const& date, string const& format)
{
   auto month = format.find("M");
   auto monthLeading = format.find("MM");
   auto day = format.find("d");
   auto dayLeading = format.find("dd");
   auto year = format.find("yyyy");

   if (monthLeading != string::npos)
   {
      month = monthLeading;
   }
   if (dayLeading != string::npos)
   {
      day = dayLeading;
   }

   string monthS;
   string dayS;
   string yearS;
   for (size_t i = 0, j = 0; i < date.size() && j < format.size(); /*inside*/)
   {
      if (j == month)
      {
         monthS.append(1, date[i]);
         ++i;
         ++j;
         if (monthLeading != string::npos)
         {
            monthS.append(1, date[i]);
            ++i;
            ++j;
         }
         else if (i < date.size() && isdigit(date[i]))
         {
            monthS.append(1, date[i]);
            ++i;
         }
      }
      else if (j == day)
      {
         dayS.append(1, date[i]);
         ++i;
         ++j;
         if (dayLeading != string::npos)
         {
            dayS.append(1, date[i]);
            ++i;
            ++j;
         }
         else if (i < date.size() && isdigit(date[i]))
         {
            dayS.append(1, date[i]);
            ++i;
         }
      }
      else if (j == year)
      {
         yearS.append(date.c_str() + i, 4);
         i += 4;
         j += 4;
      }
      else if (date[i] == format[j])
      {
         ++i;
         ++j;
      }
      else
      {
         throw std::logic_error(
                  "Date '" + date + "' doesn't match format string '" + format +
                  "'");
      }
   }

   auto monthN = strtol(monthS.c_str(), nullptr, 10);
   auto dayN = strtol(dayS.c_str(), nullptr, 10);
   auto yearN = strtoul(yearS.c_str(), nullptr, 10);

   return Date(static_cast<unsigned char>(monthN),
               static_cast<unsigned char>(dayN), yearN);
}

Date::Date(unsigned char month, unsigned char day, size_t year) :
   m_day(day),
   m_month(month),
   m_year(year)
{
   assert(month >= 1 && month <= 12);
   assert(day >= 1 && day <= daysInMonth(month, year));
}

unsigned char Date::day() const
{
   return m_day;
}

unsigned char Date::month() const
{
   return m_month;
}

size_t Date::year() const
{
   return m_year;
}

bool Date::operator<(Date const& other) const
{
   if (m_year < other.m_year)
   {
      return true;
   }
   else if (m_year == other.m_year)
   {
      if (m_month < other.m_month)
      {
         return true;
      }
      else if (m_month == other.m_month)
      {
         return m_day < other.m_day;
      }
   }
   return false;
}

bool Date::operator==(Date const& other) const
{
   return m_month == other.m_month && m_day == other.m_day &&
         m_year == other.m_year;
}

bool Date::operator<=(Date const& other) const
{
   return *this < other || *this == other;
}
