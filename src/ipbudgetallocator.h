#pragma once

#include <map>
#include <set>
#include "currency.h"
#include "daterange.h"
#include "itemprocessor.h"

class IPBudgetAllocator : public ItemProcessor
{
public:
   IPBudgetAllocator();
   Currency budgetable() const;
   void finish();
   void processItem(LedgerAccount const& account);
   bool processItem(LedgerBudget const& budget);
   void processItem(LedgerBudgetCancelEntry const& entry);
   void processItem(LedgerBudgetCloseEntry const& entry);
   void processItem(LedgerBudgetGoalEntry const& entry);
   void processItem(LedgerBudgetGoalsEntry const& entry);
   void processItem(LedgerBudgetIncomeEntry const& entry);
   void processItem(LedgerBudgetReserveAmountEntry const& entry);
   void processItem(LedgerBudgetReservePercentEntry const& entry);
   void processItem(LedgerBudgetRoutineEntry const& entry);
   void processItem(LedgerBudgetWithholdingEntry const& entry);
   bool processItem(LedgerTransactionV2 const& transaction);
   void processItem(LedgerTransactionV2CategoryEntry const& entry);
   void processItem(LedgerTransactionV2OwnerEntry const& entry);

private:
   void advanceBudgetPeriod(std::string const& filename, size_t lineNum,
                            Date const& date);
   void advanceBudgetPeriod(DateRange const& period);
   void syncGoal(std::string const& category, std::string const& goal);
   void syncReserve(std::string const& category);

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

   // TODO convert this to percentage only, then use goal for reserve-per-period
   //   change the names to make sense
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
   std::map<std::string, Currency> m_availables;
   DateRange m_currentPeriod;
   std::map<std::string, Goals> m_goals;
   std::set<std::string> m_incomes;
   std::map<std::string, std::string> m_owners;
   DateRange m_priorPeriod;
   std::map<std::string, Reserve> m_reserves;
   std::map<std::string, Routine> m_routines;
   std::set<std::string> m_withholdings;
   Date m_workingDate;
};
