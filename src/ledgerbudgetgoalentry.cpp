#include "ledgerbudgetgoalentry.h"

#include "itemprocessor.h"

LedgerBudgetGoalEntry::LedgerBudgetGoalEntry(std::string const& filename,
                                             size_t lineNum) :
   LedgerBudgetEntry(filename, lineNum)
{
}

void LedgerBudgetGoalEntry::processItem(ItemProcessor& processor) const
{
   processor.processItem(*this);
}
