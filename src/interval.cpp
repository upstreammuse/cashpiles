#include "interval.h"

#include <cassert>
#include <sstream>
#include "date.h"

Interval Interval::fromString(std::string const& interval, bool* ok)
{
   bool dummy;
   bool* success = ok ? ok : &dummy;
   *success = true;
   size_t number = std::stoull(interval.substr(1, interval.size() - 2),
                               nullptr, 10);

   std::string start = interval.substr(0, 1);
   if (start != "+")
   {
      *success = false;
   }

   std::string periodStr = interval.substr(interval.size() - 1);
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
   // TODO this constructor is evil
}

Interval::Interval(size_t number, Period period) :
   m_number(number),
   m_period(period)
{
}

Interval::Interval(Date const& start, Date const& end) :
   m_number(size_t(start.daysTo(end) + 1)),
   m_period(Period::DAYS)
{
}

size_t Interval::number() const
{
   return m_number;
}

Interval::Period Interval::period() const
{
   return m_period;
}

std::string Interval::toString() const
{
   std::stringstream result;
   result << "+";
   switch (m_period)
   {
      case Interval::Period::DAYS:
         if (m_number % 7 == 0)
         {
            result << m_number / 7 << "w";
         }
         else
         {
            result << m_number << "d";
         }
         break;
      case Interval::Period::MONTHS:
         result << m_number << "m";
         break;
      case Interval::Period::YEARS:
         result << m_number << "y";
         break;
   }
   return result.str();
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
   assert(result.isValid());
   return result;
}
