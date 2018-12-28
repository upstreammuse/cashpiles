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
