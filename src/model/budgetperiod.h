#pragma once

#include <memory>
#include "m_daterange.h"
#include "modeldata.h"

namespace model { struct BudgetPeriod; }

struct model::BudgetPeriod : public ModelData
{
   DateRange const period;  //PK
   std::shared_ptr<BudgetPeriod const> prevPeriod;  //FK-nillable
   std::shared_ptr<BudgetPeriod const> nextPeriod;  //FK-nillable

   explicit BudgetPeriod(DateRange const&);
};

