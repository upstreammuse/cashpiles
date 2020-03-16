#ifndef INTERVAL_H
#define INTERVAL_H

#include <QString>

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
   static Interval fromString(QString const& interval, bool* ok);

public:
   Interval();
   Interval(uint number, Period period);
   Interval(Date const& start, Date const& end);
   uint number() const;
   Period period() const;
   QString toString() const;

   bool operator==(Interval const& other) const;

private:
   uint m_number = 0;
   Period m_period = Period::DAYS;
};

Date operator+(Date const& left, Interval const& right);

#endif
