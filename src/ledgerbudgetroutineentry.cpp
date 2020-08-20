#include "ledgerbudgetroutineentry.h"

#include "itemprocessor.h"

void LedgerBudgetRoutineEntry::processItem(ItemProcessor& processor) const
{
   processor.processItem(*this);
}
