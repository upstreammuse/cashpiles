#pragma once

#include <memory>
#include "modeldata.h"

namespace model { struct BudgetEntry; }
namespace model { struct BudgetPeriod; }

struct model::BudgetEntry : public ModelData
{
   // TODO this doesn't seem to make sense
   std::shared_ptr<BudgetPeriod const> budget;  //FK

   explicit BudgetEntry(std::shared_ptr<BudgetPeriod const>);
};
