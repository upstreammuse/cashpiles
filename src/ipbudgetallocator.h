#pragma once

#include <map>
#include <set>
#include "currency.h"
#include "daterange.h"
#include "identifier.h"
#include "itemprocessor.h"

class IPBudgetAllocator : public ItemProcessor
{
public:
   IPBudgetAllocator(Date const& today);
   void finish();
   void processItem(LedgerAccount const& account);
   void processItem(LedgerBudget const& budget);
   void processItem(LedgerBudgetCancelEntry const& budget);
   void processItem(LedgerBudgetCloseEntry const& budget);
   void processItem(LedgerBudgetGoalEntry const& budget);
   void processItem(LedgerBudgetGoalsEntry const& budget);
   void processItem(LedgerBudgetIncomeEntry const& budget);
   void processItem(LedgerBudgetReserveAmountEntry const& budget);
   void processItem(LedgerBudgetReservePercentEntry const& budget);
   void processItem(LedgerBudgetRoutineEntry const& budget);
   void processItem(LedgerBudgetWithholdingEntry const& budget);
   void processItem(LedgerTransaction const& transaction);

private:
   void advanceBudgetPeriod(std::string const& filename, size_t lineNum,
                            Date const& date);
   void syncGoal(std::string const& category, std::string const& goal);
   void syncReserve(Identifier const& category);

private:
   struct Goal
   {
      Currency amount;
      DateRange period;
      Currency reserved;
   };

   struct Goals
   {
      std::map<std::string, Goal> goals;
      Currency spent;
   };

   struct Reserve
   {
      Currency amount;
      double percentage;
      DateRange period;
      Currency reserved;
   };

   struct Routine
   {
      Currency currentAmount;
      Currency priorAmount;
      Currency reserved;
   };

private:
   std::map <Identifier, Currency> m_availables;
   DateRange m_currentPeriod;
   std::map<std::string, Goals> m_goals;
   std::set<Identifier> m_incomes;
   std::map<Identifier, Identifier> m_owners;
   DateRange m_priorPeriod;
   std::map<Identifier, Reserve> m_reserves;
   std::map<Identifier, Routine> m_routines;
   Date m_today;
   std::set<Identifier> m_withholdings;
};
