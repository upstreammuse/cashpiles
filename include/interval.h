#ifndef INTERVAL_H
#define INTERVAL_H

class QDate;
class QString;

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
   int number() const;
   Period period() const;
   QString toString() const;

private:
   int m_number = 0;
   Period m_period = Period::DAYS;
};

QDate operator+(QDate const& left, Interval const& right);

#endif
