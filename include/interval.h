#ifndef INTERVAL_H
#define INTERVAL_H

class Interval
{
public:
   enum class Period
   {
      DAYS,
      MONTHS,
      YEARS
   };

public:
   Interval();
   Interval(int number, Period period);

private:
   int m_number = 0;
   Period m_period = Period::DAYS;
};

#endif
