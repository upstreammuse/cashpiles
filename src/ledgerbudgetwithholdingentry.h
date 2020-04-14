#pragma once

#include "ledgerbudgetentry.h"

class LedgerBudgetWithholdingEntry : public LedgerBudgetEntry
{
public:
   LedgerBudgetWithholdingEntry(std::string const& filename, size_t lineNum);
   void processItem(ItemProcessor& processor) const;
};
