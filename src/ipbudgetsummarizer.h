#pragma once

#include <list>
#include <map>
#include "currency.h"
#include "date.h"
#include "daterange.h"
#include "itemprocessor.h"
#include "report.h"

class Transaction
{
   Currency amount;
   Date date;
   std::string payee;
};

//   struct Category
//   {
//      Currency activity;
//      Currency allocated;
//      std::string owner;
//      int percentage = 0;
//      Currency startingBalance;
//      enum class Type
//      {
//         GOALS,
//         INCOME,
//         RESERVE,
//         ROUTINE,
//         WITHHOLDING
//      } type;
//   };
class Category
{
public:
   virtual ~Category();
   virtual void addTransaction(Transaction const& transaction);
};

class BudgetPeriod
{
public:
   void addTransaction(
         std::string const& category, Transaction const& transaction);

private:
   std::map<std::string, std::shared_ptr<Category>> categories;
   DateRange dates;
//   bool dirty = true;
   std::map<std::string, Currency> owners;
};

class IPBudgetSummarizer : public ItemProcessor
{
public:
   IPBudgetSummarizer(Report& report);
   void finish();
   bool processItem(LedgerBudget const& budget);
   bool processItem(LedgerTransactionV2 const& transaction);
   void processItem(LedgerTransactionV2CategoryEntry const& entry);

private:
   void setPeriod(Date const& date);

private:
   std::list<BudgetPeriod> m_budgetPeriods;
//   std::list<BudgetPeriod>::iterator m_currentPeriod;
   Report& m_report;
   Date m_workingDate = DateBuilder{}.month(1).day(1).year(1).toDate();
   std::string m_workingPayee;
};
