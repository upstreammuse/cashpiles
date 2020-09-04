#pragma once

#include <fstream>
#include <list>
#include <map>
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
};
