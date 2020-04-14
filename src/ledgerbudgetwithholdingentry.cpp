#include "ledgerbudgetwithholdingentry.h"

#include "itemprocessor.h"

LedgerBudgetWithholdingEntry::LedgerBudgetWithholdingEntry(
      std::string const& filename, size_t lineNum) :
   LedgerBudgetEntry(filename, lineNum)
{
}

void LedgerBudgetWithholdingEntry::processItem(ItemProcessor& processor) const
{
   processor.processItem(*this);
}
