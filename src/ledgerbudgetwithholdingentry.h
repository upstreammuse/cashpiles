#pragma once

#include "ledgerbudgetentry.h"

class LedgerBudgetWithholdingEntry : public LedgerBudgetEntry
{
public:
   using LedgerBudgetEntry::LedgerBudgetEntry;
   void processItem(ItemProcessor& processor) const;
};
