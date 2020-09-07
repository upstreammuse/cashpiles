#pragma once

#include <fstream>
#include <list>
#include <map>
#include <memory>
#include "currency.h"
#include "date.h"
#include "daterange.h"
#include "itemprocessor.h"

class LedgerItem;

class IPLogger : public ItemProcessor
{
public:
   IPLogger(std::string const& filename);
   void finishBudget();
   void processItem(LedgerAccount const& account);
   void processItem(LedgerAccountBalance const& account);
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

private:
   void allocateBudget();
   void log(std::string const& message);
   void moveMoney(Currency& to, Currency& from, Currency amount);
   void processDate(Date const& date, bool allocateLastBudget = true);
   void savePosition(LedgerItem const& item);
   void warn(std::string const& message);

private:
   struct Account
   {
      Currency balance;
      bool onBudget = false;
   };

   struct Goal
   {
      Currency allocated;
      Currency target;
      DateRange targetDates;
   };

   struct Goals
   {
      Currency balance;
      std::map<std::string, Goal> goals;
      std::string owner;
   };

   struct Category
   {
      Currency allocated;
      std::string owner;
      double percentage = 0;
      Currency spent;
      Currency target;
      DateRange targetDates;
      enum class Type
      {
         INCOME,
         ROUTINE,
         RESERVE_AMOUNT,
         RESERVE_PERCENTAGE,
         WITHHOLDING
      };
      Type type;
   };

   struct BudgetPeriod
   {
      std::map<std::string, Category> categories;
      DateRange dates;
      std::map<std::string, Goals> goals;
   };

private:
   std::map<std::string, Account> m_accounts;
   std::list<BudgetPeriod> m_budget;
   Date m_currentDate;
   std::ofstream m_out;
   std::map<std::string, Currency> m_owners;
   std::string m_positionInfo;
};
