#pragma once

#include "ledgerbudgetentry.h"

class LedgerBudgetIncomeEntry : public LedgerBudgetEntry
{
public:
   LedgerBudgetIncomeEntry(QString const& filename, uint lineNum);
   void processItem(ItemProcessor& processor) const;
};
