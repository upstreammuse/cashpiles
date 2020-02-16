#pragma once

#include "ledgerbudgetentry.h"

class LedgerBudgetWithholdingEntry : public LedgerBudgetEntry
{
public:
   LedgerBudgetWithholdingEntry(QString const& filename, uint lineNum);
   void processItem(ItemProcessor& processor) const;
};
