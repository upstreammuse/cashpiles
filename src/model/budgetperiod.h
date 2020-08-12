#pragma once

#include <memory>
#include "../daterange.h"
#include "modeldata.h"

namespace model { struct BudgetPeriod; }

struct model::BudgetPeriod : public ModelData
{
   int const id = newId();  //PK
   std::shared_ptr<BudgetPeriod const> prevPeriod;  //FK-nillable
   std::shared_ptr<BudgetPeriod const> nextPeriod;  //FK-nillable
   DateRange period;

   explicit BudgetPeriod(DateRange const&);
};
