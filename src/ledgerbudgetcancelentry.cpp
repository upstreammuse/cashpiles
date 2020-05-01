#include "ledgerbudgetcancelentry.h"

#include "itemprocessor.h"

using std::string;

string LedgerBudgetCancelEntry::goal() const
{
   return m_goal;
}

void LedgerBudgetCancelEntry::setGoal(string const& goal)
{
   m_goal = goal;
}

void LedgerBudgetCancelEntry::processItem(ItemProcessor& processor) const
{
   processor.processItem(*this);
}
