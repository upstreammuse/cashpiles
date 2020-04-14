#include "ledgerbudgetcloseentry.h"

#include "itemprocessor.h"

LedgerBudgetCloseEntry::LedgerBudgetCloseEntry(std::string const& filename,
                                               size_t lineNum) :
   LedgerBudgetEntry(filename, lineNum)
{
}

void LedgerBudgetCloseEntry::processItem(ItemProcessor& processor) const
{
   processor.processItem(*this);
}
