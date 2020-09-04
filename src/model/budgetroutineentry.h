#pragma once

#include "budgetcategoryentry.h"

namespace model { struct BudgetRoutineEntry; }

struct model::BudgetRoutineEntry : public BudgetCategoryEntry
{
   using BudgetCategoryEntry::BudgetCategoryEntry;
};
