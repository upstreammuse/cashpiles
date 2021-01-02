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
   static Interval fromString(std::string const& interval);

public:
   Interval(int number, Period period);
   Interval(Date const& start, Date const& end);

   int number() const;
   Period period() const;
   std::string toString() const;

   bool operator==(Interval const& other) const;

private:
   void assertValid() const;

private:
   int m_number;
   Period m_period;
};

Date operator+(Date const& left, Interval const& right);
Date operator-(Date const& left, Interval const& right);
