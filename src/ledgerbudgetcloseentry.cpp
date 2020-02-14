#include "ledgerbudgetcloseentry.h"

#include "itemprocessor.h"

LedgerBudgetCloseEntry::LedgerBudgetCloseEntry(QString const& filename,
                                               uint lineNum) :
   LedgerBudgetEntry(filename, lineNum)
{
}

void LedgerBudgetCloseEntry::processItem(ItemProcessor& processor) const
{
   processor.processItem(*this);
}
