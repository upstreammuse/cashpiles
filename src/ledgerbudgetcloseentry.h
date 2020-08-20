#pragma once

#include "ledgerbudgetentry.h"

class LedgerBudgetCloseEntry : public LedgerBudgetEntry
{
public:
   using LedgerBudgetEntry::LedgerBudgetEntry;
   void processItem(ItemProcessor& processor) const;
};
