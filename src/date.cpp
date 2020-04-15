#include "date.h"

#include <cassert>
#include <ctime>
#include <iomanip>

Date Date::currentDate()
{
   std::time_t time = std::time(nullptr);
   struct tm dateTime;

#ifdef WINNT
   localtime_s(&dateTime, &time);
#else
   localtime_s(&time, &dateTime);
#endif

   Date retval;
   retval.m_day = dateTime.tm_mday;
   retval.m_month = dateTime.tm_mon + 1;
   retval.m_year = dateTime.tm_year + 1900;
   return retval;
}

Date Date::fromString(std::string const& date, std::string const& format)
{
   size_t month = format.find("M");
   size_t monthLeading = format.find("MM");
   size_t day = format.find("d");
   size_t dayLeading = format.find("dd");
   size_t year = format.find("yyyy");

   if (monthLeading != std::string::npos)
   {
      month = monthLeading;
   }
   if (dayLeading != std::string::npos)
   {
      day = dayLeading;
   }

   std::string monthS;
   std::string dayS;
   std::string yearS;
   for (size_t i = 0, j = 0; i < date.size() && j < format.size(); /*inside*/)
   {
      if (j == month)
      {
         monthS.append(1, date[i]);
         ++i;
         ++j;
         if (monthLeading == std::string::npos)
         {
            if (i < date.size() && isdigit(date[i]))
            {
               monthS.append(1, date[i]);
               ++i;
            }
         }
         else
         {
            monthS.append(1, date[i]);
            ++i;
            ++j;
         }
      }
      else if (j == day)
      {
         dayS.append(1, date[i]);
         ++i;
         ++j;
         if (dayLeading == std::string::npos)
         {
            if (i < date.size() && isdigit(date[i]))
            {
               dayS.append(1, date[i]);
               ++i;
            }
         }
         else
         {
            dayS.append(1, date[i]);
            ++i;
            ++j;
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
         return Date();
      }
   }

   if (monthS.find_first_not_of("0123456789") != std::string::npos)
   {
      return Date();
   }
   if (dayS.find_first_not_of("0123456789") != std::string::npos)
   {
      return Date();
   }
   if (yearS.find_first_not_of("0123456789") != std::string::npos)
   {
      return Date();
   }

   Date d;
   d.m_month = strtol(monthS.c_str(), nullptr, 10);
   d.m_day = strtol(dayS.c_str(), nullptr, 10);
   d.m_year = strtol(yearS.c_str(), nullptr, 10);
   return d;
}

Date::Date()
{
}

Date::Date(int year, int month, int day) :
   m_day(day),
   m_month(month),
   m_year(year)
{
}

Date Date::addDays(int days) const
{
   Date d = *this;
   int daysInMonth[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

   // handle positive movement
   for (/*already set*/; days > 0; --days)
   {
      if (d.m_year % 400 == 0 || (d.m_year % 100 != 0 && d.m_year % 4 == 0))
      {
         daysInMonth[2] = 29;
      }
      else
      {
         daysInMonth[2] = 28;
      }

      ++d.m_day;
      if (d.m_day > daysInMonth[d.m_month])
      {
         d.m_day = 1;
         ++d.m_month;
      }
      if (d.m_month > 12)
      {
         d.m_month = 1;
         ++d.m_year;
      }
   }

   // handle negative movement
   for (/*already set*/; days < 0; ++days)
   {
      if (d.m_year % 400 == 0 || (d.m_year % 100 != 0 && d.m_year % 4 == 0))
      {
         daysInMonth[2] = 29;
      }
      else
      {
         daysInMonth[2] = 28;
      }

      --d.m_day;
      if (d.m_day < 1)
      {
         --d.m_month;
         d.m_day = daysInMonth[d.m_month];
      }
      if (d.m_month < 1)
      {
         d.m_month = 12;
         --d.m_year;
      }
   }

   return d;
}

Date Date::addMonths(int months) const
{
   // TODO figure out if this works for negative months, based on how modulo and division work
   Date d = *this;
   d.m_month += months % 12;
   while (d.m_month > 12)
   {
      d.m_month -= 12;
      d.m_year += 1;
   }
   d.m_year += months / 12;
   int daysInMonth[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
   if ((d.m_year % 4 == 0 && d.m_year % 100 != 0) || d.m_year % 400 == 0)
   {
      daysInMonth[2] = 29;
   }
   if (d.m_day > daysInMonth[d.m_month])
   {
      d.m_day = daysInMonth[d.m_month];
   }
   return d;
}

Date Date::addYears(int years) const
{
   Date d = *this;
   d.m_year += years;
   int daysInMonth[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
   if ((d.m_year % 4 == 0 && d.m_year % 100 != 0) || d.m_year % 400 == 0)
   {
      daysInMonth[2] = 29;
   }
   if (d.m_day > daysInMonth[d.m_month])
   {
      d.m_day = daysInMonth[d.m_month];
   }
   return d;
}

long long int Date::daysTo(Date const& other) const
{
   long long julianStart = toJulianDayNumber();
   long long julianEnd = other.toJulianDayNumber();
   return julianEnd - julianStart;
}

bool Date::isNull() const
{
   return m_day <= 0 || m_month <= 0 || m_year <= 0;
}

bool Date::isValid() const
{
   if (m_year <= 0) return false;
   if (m_month <= 0 || m_month > 12) return false;
   int daysInMonth[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
   if ((m_year % 4 == 0 && m_year % 100 != 0) || m_year % 400 == 0)
   {
      daysInMonth[2] = 29;
   }
   if (m_day <= 0 || m_day > daysInMonth[m_month]) return false;
   return true;
}

int Date::month() const
{
   assert(isValid());
   return m_month;
}

std::string Date::toString(std::string const& format) const
{
   if (isNull())
   {
      return "";
   }
   std::string retval = format;

   size_t monthLeading = retval.find("MM");
   size_t month = retval.find("M");
   if (monthLeading != std::string::npos)
   {
      std::stringstream ss;
      ss << retval.substr(0, monthLeading)
         << std::setw(2) << std::setfill('0') << m_month
         << retval.substr(monthLeading + 2);
      retval = ss.str();
   }
   else if (month != std::string::npos)
   {
      std::stringstream ss;
      ss << retval.substr(0, month)
         << m_month
         << retval.substr(month + 1);
      retval = ss.str();
   }

   size_t dayLeading = retval.find("dd");
   size_t day = retval.find("d");
   if (dayLeading != std::string::npos)
   {
      std::stringstream ss;
      ss << retval.substr(0, dayLeading)
         << std::setw(2) << std::setfill('0') << m_day
         << retval.substr(dayLeading + 2);
      retval = ss.str();
   }
   else if (day != std::string::npos)
   {
      std::stringstream ss;
      ss << retval.substr(0, day)
         << m_day
         << retval.substr(day + 1);
      retval = ss.str();
   }

   size_t year = retval.find("yyyy");
   if (year != std::string::npos)
   {
      std::stringstream ss;
      ss << retval.substr(0, year)
         << std::setw(4) << std::setfill('0') << m_year
         << retval.substr(year + 4);
      retval = ss.str();
   }

   return retval;
}

int Date::year() const
{
   assert(isValid());
   return m_year;
}

bool Date::operator==(Date const& other) const
{
   return m_day == other.m_day &&
         m_month == other.m_month &&
         m_year == other.m_year;
}

bool Date::operator!=(Date const& other) const
{
   return !(*this == other);
}

bool Date::operator<(Date const& other) const
{
   return toJulianDayNumber() < other.toJulianDayNumber();
}

bool Date::operator<=(Date const& other) const
{
   return *this < other || *this == other;
}

bool Date::operator>(Date const& other) const
{
   return !(*this <= other);
}

// from Wikipedia, would love to derive how this works
long long int Date::toJulianDayNumber() const
{
   return (1461 * (m_year + 4800 + (m_month - 14) / 12)) / 4 +
         (367 * (m_month - 2 - 12 * ((m_month - 14) / 12))) / 12 -
         (3 * ((m_year + 4900 + (m_month - 14) / 12) / 100)) / 4 +
         m_day - 32075;
}
