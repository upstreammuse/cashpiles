#include "m_daterange.h"

using namespace model;
using namespace util;

DateRange::DateRange(Date const& date, Interval const& interval_) :
   startDate(date),
   interval(interval_)
{
}
