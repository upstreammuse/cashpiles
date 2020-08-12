#include "ledgerbudgetgoalentry.h"

#include "itemprocessor.h"

using std::string;

LedgerBudgetGoalEntry::LedgerBudgetGoalEntry(
      Date const& date, std::string const& filename, size_t linenum) :
   LedgerBudgetEntry(filename, linenum),
   m_goalDate(date)
{
}

Currency LedgerBudgetGoalEntry::amount() const
{
   return m_amount;
}

void LedgerBudgetGoalEntry::setAmount(Currency const& amount)
{
   m_amount = amount;
}

string LedgerBudgetGoalEntry::goal() const
{
   return m_goal;
}

void LedgerBudgetGoalEntry::setGoal(string const& goal)
{
   m_goal = goal;
}

Date LedgerBudgetGoalEntry::goalDate() const
{
   return m_goalDate;
}

void LedgerBudgetGoalEntry::setGoalDate(Date const& date)
{
   m_goalDate = date;
}

void LedgerBudgetGoalEntry::processItem(ItemProcessor& processor) const
{
   processor.processItem(*this);
}
