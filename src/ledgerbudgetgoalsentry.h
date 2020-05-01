#pragma once

#include "ledgerbudgetentry.h"

class LedgerBudgetGoalsEntry : public LedgerBudgetEntry
{
public:
   using LedgerBudgetEntry::LedgerBudgetEntry;
   void processItem(ItemProcessor& processor) const;
};
