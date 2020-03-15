#ifndef DATERANGE_H
#define DATERANGE_H

#include "date.h"
#include "interval.h"

class DateRange
{
public:
   DateRange();
   DateRange(QDate const& start, QDate const& end);
   DateRange(QDate const& start, Interval const& interval);
   qint64 days() const;
   QDate endDate() const;
   DateRange intersect(DateRange const& other) const;
   bool isNull() const;
   QDate startDate() const;

   DateRange& operator++();
   bool operator==(DateRange const& other) const;

private:
   Interval m_interval;
   Date m_startDate;
};

#endif
