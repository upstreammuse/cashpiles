#pragma once

#include <map>
#include <memory>
#include "../daterange.h"
#include "modeldata.h"

namespace model { struct BudgetCategoryEntry; }
namespace model { struct BudgetPeriod; }

struct model::BudgetPeriod : public ModelData
{
   int const id = newId();  //PK
   std::shared_ptr<BudgetPeriod const> prevPeriod;  //FK-nillable
   std::shared_ptr<BudgetPeriod const> nextPeriod;  //FK-nillable
   std::map<std::string, std::shared_ptr<BudgetCategoryEntry>> entries;
   DateRange period;

   explicit BudgetPeriod(DateRange const&);
};
