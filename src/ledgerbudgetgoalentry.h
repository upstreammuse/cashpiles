#pragma once

#include "ledgerbudgetentry.h"

class LedgerBudgetGoalEntry : public LedgerBudgetEntry
{
public:
   LedgerBudgetGoalEntry(
         Date const& date, std::string const& filename, size_t linenum);
   LedgerBudgetGoalEntry(Date const& date, LedgerItem const&);

   Currency amount() const;
   void setAmount(Currency const& amount);

   std::string goal() const;
   void setGoal(std::string const& goal);

   Date goalDate() const;
   void setGoalDate(Date const& date);

   void processItem(ItemProcessor& processor) const;

private:
   Currency m_amount;
   std::string m_goal;
   Date m_goalDate;
};
