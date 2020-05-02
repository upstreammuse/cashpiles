#pragma once

#include "date.h"
#include "interval.h"

class DateRange
{
public:
   DateRange();
   DateRange(Date const& start, Date const& end);
   DateRange(Date const& start, Interval const& interval);
   long long int days() const;
   Date endDate() const;
   DateRange intersect(DateRange const& other) const;
   bool isNull() const;
   Date startDate() const;

   DateRange& operator++();
   bool operator==(DateRange const& other) const;

private:
   Interval m_interval;
   Date m_startDate;
};
