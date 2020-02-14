#pragma once

#include "ledgerbudgetentry.h"

class LedgerBudgetGoalEntry : public LedgerBudgetEntry
{
public:
   LedgerBudgetGoalEntry(QString const& filename, uint lineNum);
   void processItem(ItemProcessor& processor) const;
};
