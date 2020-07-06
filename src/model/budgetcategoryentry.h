#pragma once

#include <memory>
#include "budgetentry.h"

namespace model { struct BudgetCategoryEntry; }
namespace model { struct BudgetCategoryOwner; }
namespace model { struct BudgetPeriod; }

struct model::BudgetCategoryEntry : public BudgetEntry
{
   std::string const name;  //PK
   std::shared_ptr<BudgetCategoryOwner const> owner;  //FK

   BudgetCategoryEntry(
         std::string const&, std::shared_ptr<BudgetPeriod const>,
         std::shared_ptr<BudgetCategoryOwner const>);
};
