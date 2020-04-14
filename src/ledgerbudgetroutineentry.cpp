#include "ledgerbudgetroutineentry.h"

#include "itemprocessor.h"

LedgerBudgetRoutineEntry::LedgerBudgetRoutineEntry(std::string const& filename,
                                                   size_t lineNum) :
   LedgerBudgetEntry(filename, lineNum)
{
}

void LedgerBudgetRoutineEntry::processItem(ItemProcessor& processor) const
{
   processor.processItem(*this);
}
