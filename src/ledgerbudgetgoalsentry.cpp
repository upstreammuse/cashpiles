#include "ledgerbudgetgoalsentry.h"

#include "itemprocessor.h"

void LedgerBudgetGoalsEntry::processItem(ItemProcessor& processor) const
{
   processor.processItem(*this);
}
