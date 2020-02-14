#pragma once

#include "ledgerbudgetentry.h"

class LedgerBudgetCloseEntry : public LedgerBudgetEntry
{
public:
   LedgerBudgetCloseEntry(QString const& filename, uint lineNum);
   void processItem(ItemProcessor& processor) const;
};
