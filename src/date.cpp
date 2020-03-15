#include "date.h"

#include <QDate>

Date Date::fromQDate(QDate const& date)
{
   Date d;
   d.m_day = date.day();
   d.m_month = date.month();
   d.m_year = date.year();
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
   return julianEnd - julianStart;
}

bool Date::isNull() const
{
   return m_day <= 0 || m_month <= 0 || m_year <= 0;
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
