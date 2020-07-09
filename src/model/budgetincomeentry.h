#pragma once

#include "budgetcategoryentry.h"

namespace model { struct BudgetIncomeEntry; }

struct model::BudgetIncomeEntry : public BudgetCategoryEntry
{
   using BudgetCategoryEntry::BudgetCategoryEntry;
};
