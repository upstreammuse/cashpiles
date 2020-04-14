#pragma once

#include "ledgerbudgetentry.h"

class LedgerBudgetCloseEntry : public LedgerBudgetEntry
{
public:
   LedgerBudgetCloseEntry(std::string const& filename, size_t lineNum);
   void processItem(ItemProcessor& processor) const;
};
