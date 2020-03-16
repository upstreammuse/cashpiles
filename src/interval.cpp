#include "interval.h"

#include "date.h"

Interval Interval::fromString(QString const& interval, bool* ok)
{
   bool dummy;
   bool* success = ok ? ok : &dummy;
   uint number = interval.mid(1, interval.size() - 2).toUInt(success);

   QString start = interval.left(1);
   if (start != "+")
   {
      *success = false;
   }

   QString periodStr = interval.right(1);
   Period period;
   if (periodStr == "d")
   {
      period = Period::DAYS;
   }
   else if (periodStr == "w")
   {
      period = Period::DAYS;
      number *= 7;
   }
   else if (periodStr == "m")
   {
      period = Period::MONTHS;
   }
   else if (periodStr == "y")
   {
      period = Period::YEARS;
   }
   else
   {
      period = Period::DAYS;
      *success = false;
   }

   return Interval(number, period);
}

Interval::Interval()
{
}

Interval::Interval(uint number, Period period) :
   m_number(number),
   m_period(period)
{
}

Interval::Interval(Date const& start, Date const& end) :
   m_number(start.daysTo(end) + 1),
   m_period(Period::DAYS)
{
}

uint Interval::number() const
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
   switch (m_period)
   {
      case Interval::Period::DAYS:
         if (m_number % 7 == 0)
         {
            result += QString::number(m_number / 7) + "w";
         }
         else
         {
            result += QString::number(m_number) + "d";
         }
         break;
      case Interval::Period::MONTHS:
         result += QString::number(m_number) + "m";
         break;
      case Interval::Period::YEARS:
         result += QString::number(m_number) + "y";
         break;
   }
   return result;
}

bool Interval::operator==(Interval const& other) const
{
   return m_number == other.m_number && m_period == other.m_period;
}

Date operator+(Date const& left, Interval const& right)
{
   if (left.isNull())
   {
      return Date();
   }
   Date result;
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
