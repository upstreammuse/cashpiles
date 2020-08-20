#pragma once

#include "ledgerbudgetentry.h"

class LedgerBudgetIncomeEntry : public LedgerBudgetEntry
{
public:
   using LedgerBudgetEntry::LedgerBudgetEntry;
   void processItem(ItemProcessor& processor) const;
};
