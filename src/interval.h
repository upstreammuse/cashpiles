#ifndef INTERVAL_H
#define INTERVAL_H

#include <QtGlobal>

class QDate;

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
   Interval(QDate const& start, QDate const& end);
   uint number() const;
   Period period() const;
   QString toString() const;

   bool operator==(Interval const& other) const;

private:
   uint m_number = 0;
   Period m_period = Period::DAYS;
};

QDate operator+(QDate const& left, Interval const& right);

#endif
