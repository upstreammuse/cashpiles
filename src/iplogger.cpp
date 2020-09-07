#include "iplogger.h"

#include <cassert>
#include <iostream>
#include <sstream>
#include "ledgeraccount.h"
#include "ledgeraccountbalance.h"
#include "ledgerblank.h"
#include "ledgerbudget.h"
#include "ledgerbudgetcancelentry.h"
#include "ledgerbudgetcloseentry.h"
#include "ledgerbudgetgoalentry.h"
#include "ledgerbudgetgoalsentry.h"
#include "ledgerbudgetincomeentry.h"
#include "ledgerbudgetreserveamountentry.h"
#include "ledgerbudgetreservepercententry.h"
#include "ledgerbudgetroutineentry.h"
#include "ledgerbudgetwithholdingentry.h"

using std::endl;
using std::list;
using std::string;
using std::stringstream;
using std::to_string;

IPLogger::IPLogger(string const& filename) :
   m_currentDate{DateBuilder().day(1).month(1).year(1).toDate()}
{
   m_out.open(filename);
}

void IPLogger::finishBudget()
{
   allocateBudget();
}

void IPLogger::processItem(LedgerAccount const& account)
{
   savePosition(account);
   processDate(account.date());
   switch (account.mode())
   {
      case LedgerAccount::Mode::ON_BUDGET:
         if (m_accounts.count(account.name()))
         {
            warn("Cannot open account that was already open");
            return;
         }
         m_accounts[account.name()].onBudget = true;
         log("Opening account '" + account.name() + "' on budget");
         break;
      case LedgerAccount::Mode::OFF_BUDGET:
         if (m_accounts.count(account.name()))
         {
            warn("Cannot open account that was already open");
            return;
         }
         m_accounts[account.name()].onBudget = false;
         log("Opening account '" + account.name() + "' off budget");
         break;
      case LedgerAccount::Mode::CLOSED:
         if (!m_accounts.count(account.name()))
         {
            warn("Cannot close account that was not open");
            return;
         }
         if (!m_accounts[account.name()].balance.isZero())
         {
            warn("Cannot close account with nonzero balance of " +
                 m_accounts[account.name()].balance.toString());
            return;
         }
         m_accounts.erase(account.name());
         log("Closing account '" + account.name() + "'");
         break;
   }
}

void IPLogger::processItem(LedgerAccountBalance const& balance)
{
   savePosition(balance);
   processDate(balance.date());
   if (!m_accounts.count(balance.account()))
   {
      warn("Cannot compare balance for account '" + balance.account() +
           "' that is not open");
      return;
   }
   if (m_accounts[balance.account()].balance != balance.amount())
   {
      warn("Account balance mismatch, actual balance " +
           m_accounts[balance.account()].balance.toString() +
            ", stated balance " + balance.amount().toString());
      return;
   }
   // TODO check for uncleared transactions
   log("Account balance matched");
}

bool IPLogger::processItem(LedgerBudget const& budget)
{
   savePosition(budget);
   if (!m_budget.empty() && budget.date() <= m_budget.back().dates.endDate())
   {
      warn("Cannot change a budget period in progress");
      return false;
   }
   processDate(budget.date(), false);
   if (budget.date() != m_budget.back().dates.startDate())
   {
      warn("Can only change a budget period on its first day");
      return false;
   }

   m_budget.back().dates = {budget.date(), budget.interval()};
   log("Reconfiguring budget period start " +
       m_budget.back().dates.startDate().toString("yyyy-MM-dd") + " end " +
       m_budget.back().dates.endDate().toString("yyyy-MM-dd"));
   return true;
}

void IPLogger::processItem(LedgerBudgetCancelEntry const& entry)
{
   savePosition(entry);
   if (!m_budget.back().goals.count(entry.category()))
   {
      warn("Cannot cancel goal '" + entry.goal() +
           "' in nonexistant category '" + entry.category() + "'");
      return;
   }
   auto& theCategory = m_budget.back().goals[entry.category()];

   if (!theCategory.goals.count(entry.goal()))
   {
      warn("Cannot cancel nonexistant goal '" + entry.goal() + "'");
      return;
   }
   auto& theGoal = theCategory.goals[entry.goal()];

   Currency ownerOldBalance = m_owners[theCategory.owner];
   moveMoney(m_owners[theCategory.owner], theGoal.allocated, theGoal.allocated);
   assert(theGoal.allocated.isZero());
   theCategory.goals.erase(entry.goal());

   log("Canceling goal '" + entry.goal() + "' in category '" +
       entry.category() + "', owner '" + theCategory.owner +
       "' previous balance " + ownerOldBalance.toString() + ", new balance " +
       m_owners[theCategory.owner].toString());
}

void IPLogger::processItem(LedgerBudgetCloseEntry const& entry)
{
   savePosition(entry);
   if (m_budget.back().categories.count(entry.category()))
   {
      auto& theCategory = m_budget.back().categories[entry.category()];

      Currency ownerOldBalance = m_owners[theCategory.owner];
      moveMoney(theCategory.allocated, theCategory.spent, theCategory.spent);
      assert(theCategory.spent.isZero());
      moveMoney(m_owners[theCategory.owner], theCategory.allocated,
            theCategory.allocated);
      assert(theCategory.allocated.isZero());

      log("Closing category '" + entry.category() + "', owner '" +
          theCategory.owner + "' previous balance " +
          ownerOldBalance.toString() + ", new balance " +
          m_owners[theCategory.owner].toString());

      m_budget.back().categories.erase(entry.category());
   }
   else if (m_budget.back().goals.count(entry.category()))
   {
      auto& theCategory = m_budget.back().goals[entry.category()];

      if (!theCategory.goals.empty())
      {
         warn("Cannot close goals category '" + entry.category() +
              "' with active goals");
         return;
      }

      Currency ownerOldBalance = m_owners[theCategory.owner];
      moveMoney(m_owners[theCategory.owner], theCategory.balance,
            theCategory.balance);
      assert(theCategory.balance.isZero());

      log("Closing category '" + entry.category() + "', owner '" +
          theCategory.owner + "' previous balance " +
          ownerOldBalance.toString() + ", new balance " +
          m_owners[theCategory.owner].toString());

      m_budget.back().categories.erase(entry.category());
   }
   else
   {
      warn("Cannot close nonexistant category '" + entry.category() + "'");
      return;
   }
}

void IPLogger::processItem(LedgerBudgetGoalEntry const& entry)
{
   savePosition(entry);
   if (m_budget.back().categories.count(entry.category()))
   {
      warn("Cannot create goal in non-goals category");
      return;
   }
   if (!m_budget.back().goals.count(entry.category()))
   {
      warn("Cannot create goal in nonexistant category '" + entry.category() +
           "'");
      return;
   }
   auto& theCategory = m_budget.back().goals[entry.category()];

   if (theCategory.goals.count(entry.goal()))
   {
      warn("Cannot create goal '" + entry.goal() + "' that already exists");
      return;
   }
   auto& theGoal = theCategory.goals[entry.goal()];

   assert(theGoal.allocated.isZero());
   theGoal.target = entry.amount();
   theGoal.targetDates = DateRange{m_currentDate, entry.goalDate()};
   log("Creating goal '" + entry.goal() + "' in category '" + entry.category() +
       "' to save " + theGoal.target.toString() + " between dates " +
       theGoal.targetDates.startDate().toString("yyyy-MM-dd") + " and " +
       theGoal.targetDates.endDate().toString("yyyy-MM-dd"));
}

void IPLogger::processItem(LedgerBudgetGoalsEntry const& entry)
{
   savePosition(entry);
   if (m_budget.back().categories.count(entry.category()))
   {
      warn("Cannot create category '" + entry.category() +
           "' that already exists");
      return;
   }
   if (m_budget.back().goals.count(entry.category()))
   {
      warn("Cannot create category '" + entry.category() +
           "' that already exists");
      return;
   }
   auto& theCategory = m_budget.back().goals[entry.category()];

   assert(theCategory.balance.isZero());
   assert(theCategory.goals.empty());
   theCategory.owner = entry.owner();
   log("Creating goals category '" + entry.category() + "' for owner '" +
       theCategory.owner + "'");
}

void IPLogger::processItem(LedgerBudgetIncomeEntry const& entry)
{
   savePosition(entry);
   if (m_budget.back().categories.count(entry.category()))
   {
      warn("Cannot create category '" + entry.category() +
           "' that already exists");
      return;
   }
   if (m_budget.back().goals.count(entry.category()))
   {
      warn("Cannot create category '" + entry.category() +
           "' that already exists");
      return;
   }
   auto& theCategory = m_budget.back().categories[entry.category()];

   assert(theCategory.allocated.isZero());
   theCategory.owner = entry.owner();
   assert(theCategory.spent.isZero());
   assert(theCategory.target.isZero());
   assert(theCategory.targetDates.isNull());
   theCategory.type = Category::Type::INCOME;
   log("Creating income category '" + entry.category() + "' for owner '" +
       theCategory.owner + "'");
}

void IPLogger::processItem(LedgerBudgetReserveAmountEntry const& entry)
{
   savePosition(entry);
   if (m_budget.back().categories.count(entry.category()))
   {
      warn("Cannot create category '" + entry.category() +
           "' that already exists");
      return;
   }
   if (m_budget.back().goals.count(entry.category()))
   {
      warn("Cannot create category '" + entry.category() +
           "' that already exists");
      return;
   }
   auto& theCategory = m_budget.back().categories[entry.category()];

   assert(theCategory.allocated.isZero());
   theCategory.owner = entry.owner();
   assert(theCategory.spent.isZero());
   theCategory.target = entry.amount();
   theCategory.targetDates = {m_currentDate, entry.interval()};
   theCategory.type = Category::Type::RESERVE_AMOUNT;
   log("Creating category '" + entry.category() + "' for owner '" +
       theCategory.owner + "' to reserve " + theCategory.target.toString() +
       " between dates " +
       theCategory.targetDates.startDate().toString("yyyy-MM-dd") + " and " +
       theCategory.targetDates.endDate().toString("yyyy-MM-dd"));
}

void IPLogger::processItem(LedgerBudgetReservePercentEntry const& entry)
{
   savePosition(entry);
   if (m_budget.back().categories.count(entry.category()))
   {
      warn("Cannot create category '" + entry.category() +
           "' that already exists");
      return;
   }
   if (m_budget.back().goals.count(entry.category()))
   {
      warn("Cannot create category '" + entry.category() +
           "' that already exists");
      return;
   }
   auto& theCategory = m_budget.back().categories[entry.category()];

   assert(theCategory.allocated.isZero());
   theCategory.owner = entry.owner();
   theCategory.percentage = entry.percentage() / 100.0;
   assert(theCategory.spent.isZero());
   assert(theCategory.target.isZero());
   assert(theCategory.targetDates.isNull());
   theCategory.type = Category::Type::RESERVE_PERCENTAGE;
   log("Creating category '" + entry.category() + "' for owner '" +
       theCategory.owner + "' to reserve " + to_string(entry.percentage()) +
       "%");
}

void IPLogger::processItem(LedgerBudgetRoutineEntry const& entry)
{
   savePosition(entry);
   if (m_budget.back().categories.count(entry.category()))
   {
      warn("Cannot create category '" + entry.category() +
           "' that already exists");
      return;
   }
   if (m_budget.back().goals.count(entry.category()))
   {
      warn("Cannot create category '" + entry.category() +
           "' that already exists");
      return;
   }
   auto& theCategory = m_budget.back().categories[entry.category()];

   assert(theCategory.allocated.isZero());
   theCategory.owner = entry.owner();
   assert(theCategory.spent.isZero());
   assert(theCategory.target.isZero());
   assert(theCategory.targetDates.isNull());
   theCategory.type = Category::Type::ROUTINE;
   log("Creating category '" + entry.category() + "' for owner '" +
       theCategory.owner + "' for routine expenses");
}

void IPLogger::processItem(LedgerBudgetWithholdingEntry const& entry)
{
   savePosition(entry);
   if (m_budget.back().categories.count(entry.category()))
   {
      warn("Cannot create category '" + entry.category() +
           "' that already exists");
      return;
   }
   if (m_budget.back().goals.count(entry.category()))
   {
      warn("Cannot create category '" + entry.category() +
           "' that already exists");
      return;
   }
   auto& theCategory = m_budget.back().categories[entry.category()];

   assert(theCategory.allocated.isZero());
   theCategory.owner = entry.owner();
   assert(theCategory.spent.isZero());
   assert(theCategory.target.isZero());
   assert(theCategory.targetDates.isNull());
   theCategory.type = Category::Type::WITHHOLDING;
   log("Creating category '" + entry.category() + "' for owner '" +
       theCategory.owner + "' for withheld income");
}

void IPLogger::allocateBudget()
{
   log("Allocating budget categories for period " +
       m_budget.back().dates.startDate().toString("yyyy-MM-dd") + " to " +
       m_budget.back().dates.endDate().toString("yyyy-MM-dd"));

   for (auto& it : m_budget.back().categories)
   {
      switch (it.second.type)
      {
         case Category::Type::INCOME:
         case Category::Type::ROUTINE:
         case Category::Type::RESERVE_AMOUNT:
         case Category::Type::RESERVE_PERCENTAGE:
         case Category::Type::WITHHOLDING:
            break;
      }
   }

   for (auto& it : m_budget.back().goals)
   {
      list<string> autoClose;
      for (auto& it2 : it.second.goals)
      {
         Currency toAllocate = it2.second.target.amortize(
                                  it2.second.targetDates,
                                  m_budget.back().dates);
         Currency oldOwnerBalance = m_owners[it.second.owner];
         moveMoney(it2.second.allocated, m_owners[it.second.owner], toAllocate);

         log("Allocating " + toAllocate.toString() + " for goal '" + it2.first +
             "' in category '" + it.first + "', goal balance is " +
             it2.second.allocated.toString() + ", owner '" + it.second.owner +
             "' previous balance " + oldOwnerBalance.toString() +
             " new balance " + m_owners[it.second.owner].toString());

         if (it2.second.targetDates.endDate() <= m_budget.back().dates.endDate())
         {
            assert(it2.second.allocated == it2.second.target);
            autoClose.push_back(it2.first);
         }
         else if (it2.second.allocated == it2.second.target)
         {
            assert(it2.second.targetDates.endDate() <= m_budget.back().dates.endDate());
            autoClose.push_back(it2.first);
         }
      }

      for (auto& goal : autoClose)
      {
         assert(it.second.goals[goal].targetDates.endDate() <= m_budget.back().dates.endDate());
         assert(it.second.goals[goal].allocated == it.second.goals[goal].target);
         Currency oldCatBalance = it.second.balance;

         moveMoney(it.second.balance, it.second.goals[goal].allocated, it.second.goals[goal].allocated);
         assert(it.second.goals[goal].allocated.isZero());
         it.second.goals.erase(goal);

         log("Goal '" + goal + "' reached, transferring to category '" +
             it.first + "', category balance was " + oldCatBalance.toString() +
             " new balance " + it.second.balance.toString());
      }
   }
}

void IPLogger::log(string const& message)
{
   m_out << m_positionInfo << message << endl;
}

void IPLogger::moveMoney(Currency& to, Currency& from, Currency amount)
{
   to += amount;
   from -= amount;
}

void IPLogger::processDate(Date const& date, bool allocateLastBudget)
{
   if (date < m_currentDate)
   {
      // TODO this is why throwing is good, this needs to return from the parent
      // that called it
      warn("Cannot use entry with out-of-order date " +
           date.toString("yyyy-MM-dd"));
   }
   if (m_budget.empty())
   {
      BudgetPeriod period;
      period.dates = {
         DateBuilder{}.month(date.month()).day(1).year(date.year()).toDate(),
         {1, Interval::Period::MONTHS}
      };
      m_budget.push_back(period);
      log("Initializing default 1-month budget starting on " +
          period.dates.startDate().toString("yyyy-MM-dd"));
   }
   while (m_budget.back().dates.endDate() < date)
   {
      BudgetPeriod period = m_budget.back();
      ++period.dates;
      m_budget.push_back(period);
      log("Advancing budget to start date " +
          period.dates.startDate().toString("yyyy-MM-dd") + " end date " +
          period.dates.endDate().toString("yyyy-MM-dd"));
      if (m_budget.back().dates.endDate() < date ||
          allocateLastBudget)
      {
         allocateBudget();
      }
   }

   m_currentDate = date;
}

void IPLogger::savePosition(const LedgerItem &item)
{
   stringstream ss;
   ss << "File '" << item.fileName() << "' Line " << item.lineNum() << ": ";
   m_positionInfo = ss.str();
}

void IPLogger::warn(string const& message)
{
   m_out << m_positionInfo << "WARNING " << message << endl;
}
