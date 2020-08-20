#include "ledgerbudgetincomeentry.h"

#include "itemprocessor.h"

void LedgerBudgetIncomeEntry::processItem(ItemProcessor& processor) const
{
   processor.processItem(*this);
}
