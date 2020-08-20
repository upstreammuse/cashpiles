#pragma once

#include "ledgerbudgetentry.h"

class LedgerBudgetRoutineEntry : public LedgerBudgetEntry
{
public:
   using LedgerBudgetEntry::LedgerBudgetEntry;
   void processItem(ItemProcessor& processor) const;
};
