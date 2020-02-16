#include "ledgerbudgetwithholdingentry.h"

#include "itemprocessor.h"

LedgerBudgetWithholdingEntry::LedgerBudgetWithholdingEntry(
      QString const& filename, uint lineNum) :
   LedgerBudgetEntry(filename, lineNum)
{
}

void LedgerBudgetWithholdingEntry::processItem(ItemProcessor& processor) const
{
   processor.processItem(*this);
}
