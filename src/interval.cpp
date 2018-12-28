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

int Interval::number() const
{
   return m_number;
}

Interval::Period Interval::period() const
{
   return m_period;
}

int Interval::toApproximateDays() const
{
   switch (m_period)
   {
      case Interval::Period::DAYS:
         return m_number;
      case Interval::Period::MONTHS:
         return qRound(m_number * 365.25 / 12.0);
      case Interval::Period::YEARS:
         return qRound(m_number * 365.25);
   }
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
