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
   void processItem(LedgerAccount const& account);
   void processItem(LedgerAccountBalance const& account);
   bool processItem(LedgerBudget const& budget);
   void processItem(LedgerBudgetCancelEntry const& entry);
   void processItem(LedgerBudgetCloseEntry const& entry);

private:
   void log(LedgerItem const& item, std::string const& message);
   void processDate(LedgerItem const& item, Date const& date);
   void warn(LedgerItem const& item, std::string const& message);

private:
   struct Account
   {
      Currency balance;
      bool onBudget = false;
   };

   struct Category
   {
      Currency allocated;
      Currency balance;
      std::map<std::string, Category> categories;
      std::string owner;
      Currency spent;
      Currency target;
      DateRange targetDates;
      enum class Type
      {
         GOAL,
         GOALS,
         INCOME,
         ROUTINE,
         RESERVE_AMOUNT,
         RESERVE_PERCENTAGE
      };
      Type type;
   };

   struct BudgetPeriod
   {
      std::map<std::string, Category> categories;
      DateRange dates;
   };

private:
   std::map<std::string, Account> m_accounts;
   std::list<BudgetPeriod> m_budget;
   Date m_currentDate;
   std::ofstream m_out;
   std::map<std::string, Currency> m_owners;
};
