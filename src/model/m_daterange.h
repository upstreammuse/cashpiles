#pragma once

#include "m_date.h"
#include "m_interval.h"

namespace model { struct DateRange; }

struct model::DateRange
{
   Date startDate;
   Interval interval;

   DateRange(Date const&, Interval const&);
};
