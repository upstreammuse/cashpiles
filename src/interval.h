#pragma once

#include <string>

class Date;

class Interval
{
public:
   enum class Period
   {
      DAYS,
      MONTHS,
      YEARS
   };
   static Interval fromString(std::string const& interval, bool* ok);

public:
   Interval();
   Interval(size_t number, Period period);
   Interval(Date const& start, Date const& end);
   size_t number() const;
   Period period() const;
   std::string toString() const;

   bool operator==(Interval const& other) const;

private:
   size_t m_number = 0;
   Period m_period = Period::DAYS;
};

Date operator+(Date const& left, Interval const& right);
