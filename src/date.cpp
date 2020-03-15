#include "date.h"

#include <ctime>
#include <QDate>

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
   Q_ASSERT(retval.toQDate() == QDate::currentDate());
   return retval;
}

Date Date::fromQDate(QDate const& date)
{
   Date d;
   d.m_day = date.day();
   d.m_month = date.month();
   d.m_year = date.year();
   Q_ASSERT(d.toQDate() == date);
   return d;
}

Date Date::fromString(std::string const& date, std::string const& format)
{
   size_t month = format.find_first_of("M");
   size_t monthLeading = format.find_first_of("MM");
   size_t day = format.find_first_of("d");
   size_t dayLeading = format.find_first_of("dd");
   size_t year = format.find_first_of("yyyy");

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
         dayS.append(date.c_str() + i, 4);
         i += 4;
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
   Q_ASSERT(d.toQDate() == QDate::fromString(QString::fromStdString(format)));
   return d;
}

long long int Date::daysTo(Date const& other) const
{
   long long julianStart = toJulianDayNumber();
   long long julianEnd = other.toJulianDayNumber();
   if (julianEnd - julianStart != toQDate().daysTo(other.toQDate()))
   {
      printf("start date %d/%d/%d, end date %d/%d/%d, daysTo %lld\n",
             m_month, m_day, m_year, other.m_month, other.m_day, other.m_year,
             julianEnd - julianStart);
      printf("start qdate %s, end qdate %s, daysTo %lld\n",
             toQDate().toString().toStdString().c_str(),
             other.toQDate().toString().toStdString().c_str(),
             toQDate().daysTo(other.toQDate()));
   }
   Q_ASSERT(julianEnd - julianStart == toQDate().daysTo(other.toQDate()));
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

QDate Date::toQDate() const
{
   return QDate(m_year, m_month, m_day);
}

bool Date::operator==(Date const& other) const
{
   return m_day == other.m_day &&
         m_month == other.m_month &&
         m_year == other.m_year;
}

long long int Date::toJulianDayNumber() const
{
   return (1461 * (m_year + 4800 + (m_month - 14) / 12)) / 4 +
         (367 * (m_month - 2 - 12 * ((m_month - 14) / 12))) / 12 -
         (3 * ((m_year + 4900 + (m_month - 14) / 12) / 100)) / 4 +
         m_day - 32075;
}
