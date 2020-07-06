#pragma once

#include "budgetentry.h"

namespace model { struct BudgetCancelEntry; }
namespace model { struct BudgetGoalEntry; }

struct model::BudgetCancelEntry : public BudgetEntry
{
   std::shared_ptr<BudgetGoalEntry const> const goal;  //PK/FK

   BudgetCancelEntry(std::shared_ptr<BudgetGoalEntry const>);
};
