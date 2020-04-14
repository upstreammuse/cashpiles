#pragma once

#include "ledgerbudgetentry.h"

class LedgerBudgetGoalEntry : public LedgerBudgetEntry
{
public:
   LedgerBudgetGoalEntry(std::string const& filename, size_t lineNum);
   void processItem(ItemProcessor& processor) const;
};
