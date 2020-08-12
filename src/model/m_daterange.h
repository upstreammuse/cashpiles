#pragma once

#include "../date.h"
#include "../interval.h"

namespace model { struct DateRange; }

struct model::DateRange
{
   Date startDate;
   Interval interval;

   DateRange(Date const&, Interval const&);
};
