#include "iplogger.h"

#include <cassert>
#include <iostream>
#include "ledgeraccount.h"
#include "ledgeraccountbalance.h"
#include "ledgerbudget.h"
#include "ledgerbudgetcancelentry.h"
#include "ledgerbudgetcloseentry.h"

using std::endl;
using std::string;

IPLogger::IPLogger(string const& filename) :
   m_currentDate{DateBuilder().day(1).month(1).year(1).toDate()}
{
   m_out.open(filename);
}

void IPLogger::processItem(LedgerAccount const& account)
{
   processDate(account, account.date());
   switch (account.mode())
   {
      case LedgerAccount::Mode::ON_BUDGET:
         if (m_accounts.count(account.name()))
         {
            warn(account, "Cannot open account that was already open");
            return;
         }
         m_accounts[account.name()].onBudget = true;
         log(account, "Opening account '" + account.name() + "' on budget");
         break;
      case LedgerAccount::Mode::OFF_BUDGET:
         if (m_accounts.count(account.name()))
         {
            warn(account, "Cannot open account that was already open");
            return;
         }
         m_accounts[account.name()].onBudget = false;
         log(account, "Opening account '" + account.name() + "' off budget");
         break;
      case LedgerAccount::Mode::CLOSED:
         if (!m_accounts.count(account.name()))
         {
            warn(account, "Cannot close account that was not open");
            return;
         }
         if (!m_accounts[account.name()].balance.isZero())
         {
            warn(account,
                "Cannot close account with nonzero balance of " +
                m_accounts[account.name()].balance.toString());
            return;
         }
         m_accounts.erase(account.name());
         log(account, "Closing account '" + account.name() + "'");
         break;
   }
}

void IPLogger::processItem(LedgerAccountBalance const& balance)
{
   processDate(balance, balance.date());
   if (!m_accounts.count(balance.account()))
   {
      warn(balance,
           "Cannot compare balance for account '" + balance.account() +
           "' that is not open");
      return;
   }
   if (m_accounts[balance.account()].balance != balance.amount())
   {
      warn(balance,
           "Account balance mismatch, actual balance " +
           m_accounts[balance.account()].balance.toString() +
            ", stated balance " + balance.amount().toString());
      return;
   }
   // TODO check for uncleared transactions
   log(balance, "Account balance matched");
}

bool IPLogger::processItem(LedgerBudget const& budget)
{
   processDate(budget, budget.date());
   if (m_budget.empty())
   {
      BudgetPeriod period;
      period.dates = {budget.date(), budget.interval()};
      m_budget.push_back(period);
   }
   else
   {
      if (budget.date() <= m_budget.back().dates.endDate())
      {
         warn(budget, "Cannot change a budget period already in progress");
         return false;
      }
      while (m_budget.back().dates.endDate() < budget.date())
      {
         BudgetPeriod period = m_budget.back();
         for (auto it : period.categories)
         {
            it.second.allocated = {};
            assert(it.second.allocated.isZero());
            it.second.spent = {};
            assert(it.second.spent.isZero());
         }
         ++period.dates;
         m_budget.push_back(period);
         log(budget, "Creating budget period from " +
             m_budget.back().dates.startDate().toString("yyyy-MM-dd") + " to " +
             m_budget.back().dates.endDate().toString("yyyy-MM-dd"));
      }
   }

   if (budget.date() != m_budget.back().dates.startDate())
   {
      warn(budget, "Can only change a budget period on its first day");
      return false;
   }

   // TODO need a way to know that we are done getting updated budget entries so
   // that we can allocate in the remaining (and new) categories

   return true;
}

// TODO this is kind of a stupid extra, why not just have the close entry cover the category and the individual goals?
//   better yet, why not consolidate the categories into a single consistent representation instead of all the different types?
void IPLogger::processItem(LedgerBudgetCancelEntry const& entry)
{
   if (!m_budget.back().categories.count(entry.category()))
   {
      warn(entry,
           "Cannot cancel goal '" + entry.goal() +
           "' in nonexistant category '" + entry.category() + "'");
      return;
   }
   auto& theCategory = m_budget.back().categories[entry.category()];

   if (theCategory.type != Category::Type::GOALS)
   {
      warn(entry,
           "Cannot cancel goal '" + entry.goal() + "' in non-goals category '" +
           entry.category() + "'");
      return;
   }
   if (!theCategory.categories.count(entry.goal()))
   {
      warn(entry, "Cannot cancel nonexistant goal '" + entry.goal() + "'");
      return;
   }
   auto& theGoal = theCategory.categories[entry.goal()];

   log(entry,
       "Canceling goal '" + entry.goal() + "' in category '" +
       entry.category() + "'");
   Currency ownerOldBalance = m_owners[theCategory.owner];
   m_owners[theCategory.owner] += theGoal.balance;
   log(entry,
       "Owner '" + theCategory.owner + "' previous balance " +
       ownerOldBalance.toString() + ", new balance " +
       m_owners[theCategory.owner].toString());
   theCategory.categories.erase(entry.goal());
}

void IPLogger::processItem(LedgerBudgetCloseEntry const& entry)
{
   if (!m_budget.back().categories.count(entry.category()))
   {
      warn(entry,
           "Cannot close nonexistant category '" + entry.category() + "'");
      return;
   }
   auto& theCategory = m_budget.back().categories[entry.category()];

   log(entry, "Closing category '" + entry.category() + "'");
   Currency ownerOldBalance = m_owners[theCategory.owner];
   m_owners[theCategory.owner] += theCategory.balance;
   log(entry,
       "Owner '" + theCategory.owner + "' previous balance " +
       ownerOldBalance.toString() + ", new balance " +
       m_owners[theCategory.owner].toString());
   m_budget.back().categories.erase(entry.category());
}

void IPLogger::log(LedgerItem const& item, string const& message)
{
   m_out << "File '" << item.fileName() << "' Line " << item.lineNum() << ": "
         << message << endl;
}

void IPLogger::processDate(LedgerItem const& item, Date const& date)
{
   if (date < m_currentDate)
   {
      log(item,
          "Cannot use entry with out-of-order date " +
          date.toString("yyyy-MM-dd"));
   }
}

void IPLogger::warn(LedgerItem const& item, string const& message)
{
   m_out << "File '" << item.fileName() << "' Line " << item.lineNum()
         << ": WARNING " << message << endl;
}
