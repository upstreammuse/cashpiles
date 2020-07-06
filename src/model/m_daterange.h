#pragma once

#include "../util/m_date.h"
#include "m_interval.h"

namespace model { struct DateRange; }

struct model::DateRange
{
   util::Date startDate;
   Interval interval;

   DateRange(util::Date const&, Interval const&);
};
