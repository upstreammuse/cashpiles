#pragma once

#include "ledgerbudgetentry.h"

class LedgerBudgetIncomeEntry : public LedgerBudgetEntry
{
public:
   LedgerBudgetIncomeEntry(std::string const& filename, size_t lineNum);
   void processItem(ItemProcessor& processor) const;
};
