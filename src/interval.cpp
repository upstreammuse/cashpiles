#include "interval.h"

#include <cassert>
#include <sstream>
#include "date.h"

Interval Interval::fromString(std::string const& interval)
{
   auto number = stol(interval.substr(1, interval.size() - 2), nullptr, 10);

   auto start = interval.substr(0, 1);
   if (start != "+")
   {
      throw std::logic_error(
               "Interval '" + interval + "' did not start with '+'");
   }

   auto periodStr = interval.substr(interval.size() - 1);
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
      throw std::logic_error(
               "Interval '" + interval + "' period not one of [dwmy]");
   }

   return Interval(number, period);
}

Interval::Interval(int number, Period period) :
   m_number(number),
   m_period(period)
{
   assertValid();
}

Interval::Interval(Date const& start, Date const& end) :
   m_number(start.daysTo(end) + 1),
   m_period(Period::DAYS)
{
   assertValid();
}

int Interval::number() const
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

void Interval::assertValid() const
{
   assert(m_number >= 0);
   switch (m_period)
   {
      case Interval::Period::DAYS:
      case Interval::Period::MONTHS:
      case Interval::Period::YEARS:
         return;
   }
   assert(false);
}

Date operator+(Date const& left, Interval const& right)
{
   switch (right.period())
   {
      case Interval::Period::DAYS:
         return left.addDays(right.number());
      case Interval::Period::MONTHS:
         return left.addMonths(right.number());
      case Interval::Period::YEARS:
         return left.addYears(right.number());
   }
   assert(false);
}
