#pragma once

#include <fstream>
#include <list>
#include <map>
#include <memory>
#include "currency.h"
#include "date.h"
#include "daterange.h"
#include "itemprocessor.h"

// TODO instead of making this spit out strings to a file, it could 'emit' a set
// of log structs that provide well-formed access to the data we are logging
// - each struct can print itself into a human readable log string like we have
//   now
// - each struct can be analyzed programmatically by a test program to make sure
//   that a piece of ledger data is correctly handled by the main program
// - each struct could be formatted into nice output ... look to see if the
//   existing report system could work for this instead of having to make
//   something new

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
   void processItem(LedgerTransaction const& transaction);
   bool processItem(LedgerTransactionV2 const& transaction);
   void processItem(LedgerTransactionV2AccountEntry const& entry);
   void processItem(LedgerTransactionV2CategoryEntry const& entry);
   void processItem(LedgerTransactionV2OwnerEntry const& entry);

private:
   struct Account
   {
      Currency balance;
      bool onBudget = false;
   };

   struct Goal
   {
      Currency allocated;
      bool repeat = true;
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
      enum class Type
      {
         INCOME,
         ROUTINE,
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
   void allocateBudget();
   template<typename T>
   auto allocateGoal(
         std::string const& categoryName, Goals& category,
         std::string const& goalName, Goal& goal, T& it);
   void allocateGoals(std::string const& categoryName, Goals& category);
   void log(std::string const& message);
   void moveMoney(Currency& to, Currency& from, Currency amount);
   void processDate(Date const& date, bool allocateLastBudget = true);
   void savePosition(LedgerItem const& item);
   void warn(std::string const& message);

private:
   std::map<std::string, Account> m_accounts;
   std::list<BudgetPeriod> m_budget;
   Date m_currentDate;
   std::ofstream m_out;
   std::map<std::string, Currency> m_owners;
   std::string m_positionInfo;
};
