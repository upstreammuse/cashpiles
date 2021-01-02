#include "ipbudgetsummarizer.h"

#include <algorithm>
#include <cassert>
#include <iomanip>
#include <iostream>
#include <map>
#include "cashpiles.h"
#include "ledgerbudget.h"
#include "ledgertransactionv2.h"

using std::cout;
using std::endl;
using std::left;
using std::map;
using std::max;
using std::right;
using std::setw;
using std::sort;
using std::string;

IPBudgetSummarizer::IPBudgetSummarizer(Report& report) :
   m_report{report}
{
}

void IPBudgetSummarizer::finish()
{
   // TODO use this to inform pulling data from category classes into tables
   /*
   for (auto period : m_budgetPeriods)
   {
      cout << "BUDGET PERIOD   "
           << period.dates.startDate().toString("MM/dd/yyyy") << " -- "
           << period.dates.endDate().toString("MM/dd/yyyy") << endl;
      map<string, Currency> categories;
      for (auto transaction : period.transactions)
      {
         categories[transaction.category] += transaction.amount;
      }
      auto maxCategoryWidth = string{"CATEGORY"}.size();
      auto maxActivityWidth = string{"ACTIVITY"}.size();
      for (auto category : categories)
      {
         maxCategoryWidth = max(maxCategoryWidth, category.first.size());
         maxActivityWidth = max(maxActivityWidth,
                                category.second.toString().size());
      }
      cout << setw(int(maxCategoryWidth) + 3) << left << "CATEGORY"
           << setw(int(maxActivityWidth)) << right << "ACTIVITY" << endl;
      for (auto category : categories)
      {
         cout << setw(int(maxCategoryWidth) + 3) << left << category.first
              << setw(int(maxActivityWidth)) << right
              << category.second.toString() << endl;
      }
   }
   */
}

bool IPBudgetSummarizer::processItem(LedgerBudget const& budget)
{
   setPeriod(budget.date());
   if (m_budgetPeriods.back().dates.startDate() != budget.date())
   {
      die(budget.fileName(), budget.lineNum(),
          "Only the current budget period can be configured, and it must "
          "happen at the beginning of the period");
   }
   m_currentPeriod->dates = {budget.date(), budget.interval()};
   return false;
}

bool IPBudgetSummarizer::processItem(LedgerTransactionV2 const& transaction)
{
   setPeriod(transaction.date());
   m_workingDate = transaction.date();
   m_workingPayee = transaction.payee();
   return true;
}

void IPBudgetSummarizer::processItem(
      LedgerTransactionV2CategoryEntry const& entry)
{
   assert(entry.amount().second);
   if (!m_currentPeriod->categories.count(entry.category()))
   {
      die(entry.fileName(), entry.lineNum(), "Unknown category");
   }
   auto category = m_currentPeriod->categories[entry.category()];
   category->addTransaction(
            Transaction{entry.amount().first, m_workingDate, m_workingPayee});
}

void IPBudgetSummarizer::setPeriod(Date const& date)
{
   if (m_budgetPeriods.empty())
   {
      BudgetPeriod period;
      period.dates = {
         DateBuilder{}.month(date.month()).day(1).year(date.year()).toDate(),
         {1, Interval::Period::MONTHS}
      };
      m_budgetPeriods.push_back(period);
   }
   while (date < m_budgetPeriods.front().dates.startDate())
   {
      BudgetPeriod period;
      period.dates = m_budgetPeriods.front().dates;
      --period.dates;
      m_budgetPeriods.push_front(period);
   }
   while (m_budgetPeriods.back().dates.endDate() < date)
   {
      BudgetPeriod period;
      period.dates = m_budgetPeriods.back().dates;
      ++period.dates;
      m_budgetPeriods.push_back(period);
   }
   for (auto it = m_budgetPeriods.begin(); it != m_budgetPeriods.end(); ++it)
   {
      if (it->dates.startDate() <= date && date <= it->dates.endDate())
      {
         m_currentPeriod = it;
         return;
      }
   }
}
