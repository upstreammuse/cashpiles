#include "ledgerbudgetgoalentry.h"

#include "itemprocessor.h"

LedgerBudgetGoalEntry::LedgerBudgetGoalEntry(QString const& filename,
                                             uint lineNum) :
   LedgerBudgetEntry(filename, lineNum)
{
}

void LedgerBudgetGoalEntry::processItem(ItemProcessor& processor) const
{
   processor.processItem(*this);
}
