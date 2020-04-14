#pragma once

#include "ledgerbudgetentry.h"

class LedgerBudgetRoutineEntry : public LedgerBudgetEntry
{
public:
   LedgerBudgetRoutineEntry(std::string const& filename, size_t lineNum);
   void processItem(ItemProcessor& processor) const;
};
