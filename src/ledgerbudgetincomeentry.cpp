#include "ledgerbudgetincomeentry.h"

#include "itemprocessor.h"

LedgerBudgetIncomeEntry::LedgerBudgetIncomeEntry(QString const& filename,
                                                 uint lineNum) :
   LedgerBudgetEntry(filename, lineNum)
{
}

void LedgerBudgetIncomeEntry::processItem(ItemProcessor& processor) const
{
   processor.processItem(*this);
}
