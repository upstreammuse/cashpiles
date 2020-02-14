#pragma once

#include "ledgerbudgetentry.h"

class LedgerBudgetRoutineEntry : public LedgerBudgetEntry
{
public:
   LedgerBudgetRoutineEntry(QString const& filename, uint lineNum);
   void processItem(ItemProcessor& processor) const;
};
