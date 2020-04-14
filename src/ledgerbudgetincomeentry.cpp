#include "ledgerbudgetincomeentry.h"

#include "itemprocessor.h"

LedgerBudgetIncomeEntry::LedgerBudgetIncomeEntry(std::string const& filename,
                                                 size_t lineNum) :
   LedgerBudgetEntry(filename, lineNum)
{
}

void LedgerBudgetIncomeEntry::processItem(ItemProcessor& processor) const
{
   processor.processItem(*this);
}
