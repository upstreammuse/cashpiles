#include "ledgerbudgetroutineentry.h"

#include "itemprocessor.h"

LedgerBudgetRoutineEntry::LedgerBudgetRoutineEntry(QString const& filename,
                                                   uint lineNum) :
   LedgerBudgetEntry(filename, lineNum)
{
}

void LedgerBudgetRoutineEntry::processItem(ItemProcessor& processor) const
{
   processor.processItem(*this);
}
