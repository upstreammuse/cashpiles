#include "interval.h"

#include <QDate>

Interval::Interval()
{
}

Interval::Interval(int number, Period period) :
   m_number(number),
   m_period(period)
{
}

Interval::Interval(QDate const& start, QDate const& end) :
   m_number(start.daysTo(end) + 1),
   m_period(Period::DAYS)
{
}

int Interval::number() const
{
   return m_number;
}

Interval::Period Interval::period() const
{
   return m_period;
}

QString Interval::toString() const
{
   QString result = "+";
   result += QString::number(m_number);
   switch (m_period)
   {
      case Interval::Period::DAYS:
         result += "d";
         break;
      case Interval::Period::MONTHS:
         result += "m";
         break;
      case Interval::Period::YEARS:
         result += "y";
         break;
   }
   return result;
}

bool Interval::operator==(Interval const& other) const
{
   return m_number == other.m_number && m_period == other.m_period;
}

QDate operator+(QDate const& left, Interval const& right)
{
   QDate result;
   switch (right.period())
   {
      case Interval::Period::DAYS:
         result = left.addDays(right.number());
         break;
      case Interval::Period::MONTHS:
         result = left.addMonths(right.number());
         break;
      case Interval::Period::YEARS:
         result = left.addYears(right.number());
         break;
   }
   return result;
}
