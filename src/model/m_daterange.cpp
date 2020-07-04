#include "m_daterange.h"

using namespace model;

DateRange::DateRange(Date const& date, Interval const& interval_) :
   startDate(date),
   interval(interval_)
{
}
