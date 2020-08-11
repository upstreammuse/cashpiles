#include "date.h"

#include <cassert>
#include <iomanip>
#include <sstream>

using std::string;

int Date::daysInMonth(int month, int year)
{
   Date d;
   d.m_day = 1;
   d.m_month = month;
   d.m_year = year;
   d.assertValid();

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

Date Date::fromString(string const& date, string const& format)
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
         throw std::logic_error("Date '" + date +
                                "' doesn't match format string '" + format +
                                "'");
      }
   }

   auto monthN = strtol(monthS.c_str(), nullptr, 10);
   auto dayN = strtol(dayS.c_str(), nullptr, 10);
   auto yearN = strtoul(yearS.c_str(), nullptr, 10);

   return DateBuilder().month(monthN).day(dayN).year(yearN).toDate();
}

bool Date::isLeapYear(int year)
{
   Date d;
   d.m_day = 1;
   d.m_month = 1;
   d.m_year = year;
   d.assertValid();
   // leap years occur every 4 years, but not every 100, but every 400
   return (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;
}

int Date::day() const
{
   return m_day;
}

int Date::month() const
{
   return m_month;
}

int Date::year() const
{
   return m_year;
}

std::string Date::toString(std::string const& format) const
{
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
   d.assertValid();

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
         if (d.m_month < 1)
         {
            d.m_month = 12;
            --d.m_year;
         }
         d.m_day = daysInMonth[d.m_month];
      }
   }
   d.assertValid();

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
   d.assertValid();
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
   d.assertValid();
   return d;
}

long long int Date::daysTo(Date const& other) const
{
   long long julianStart = toJulianDayNumber();
   long long julianEnd = other.toJulianDayNumber();
   return julianEnd - julianStart;
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

   // TODO or consider this, which may be less computational, but is also more verbose...
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

bool Date::operator<=(Date const& other) const
{
   return *this < other || *this == other;
}

bool Date::operator>(Date const& other) const
{
   return !(*this <= other);
}

void Date::assertValid()
{
   assert(m_month >= 1 && m_month <= 12);
   assert(m_year >= 1);
   assert(m_day >= 1 && m_day <= daysInMonth(m_month, m_year));
}

// TODO rework this to use something I can explain a little easier, even if not strictly Julian
// from Wikipedia, would love to derive how this works
long long int Date::toJulianDayNumber() const
{
   return (1461 * (m_year + 4800 + (m_month - 14) / 12)) / 4 +
   (367 * (m_month - 2 - 12 * ((m_month - 14) / 12))) / 12 -
   (3 * ((m_year + 4900 + (m_month - 14) / 12) / 100)) / 4 +
   m_day - 32075;
}

Date DateBuilder::toDate() const
{
   Date d;
   d.m_day = m_day;
   d.m_month = m_month;
   d.m_year = m_year;
   d.assertValid();
   return d;
}

DateBuilder& DateBuilder::day(int day)
{
   m_day = day;
   return *this;
}

DateBuilder& DateBuilder::month(int month)
{
   m_month = month;
   return *this;
}

DateBuilder& DateBuilder::year(int year)
{
   m_year = year;
   return *this;
}
