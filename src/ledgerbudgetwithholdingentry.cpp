#include "ledgerbudgetwithholdingentry.h"

#include "itemprocessor.h"

void LedgerBudgetWithholdingEntry::processItem(ItemProcessor& processor) const
{
   processor.processItem(*this);
}
