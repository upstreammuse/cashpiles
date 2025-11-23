#pragma once

#include "ledgerbudgetentry.h"

class LedgerBudgetCancelEntry : public LedgerBudgetEntry
{
public:
   using LedgerBudgetEntry::LedgerBudgetEntry;

   std::string goal() const;
   void setGoal(std::string const& goal);

   void processItem(ItemProcessor& processor) const;

private:
   std::string m_goal;
};
