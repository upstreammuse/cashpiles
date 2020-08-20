#include "ledgerbudgetcloseentry.h"

#include "itemprocessor.h"

void LedgerBudgetCloseEntry::processItem(ItemProcessor& processor) const
{
   processor.processItem(*this);
}
