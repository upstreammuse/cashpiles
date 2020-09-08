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
   theGoal.repeat = false;
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
   auto& theCategory = m_budget.back().goals[entry.category()];
   auto& theGoal = theCategory.goals[entry.category()];

   assert(theCategory.balance.isZero());
   theCategory.owner = entry.owner();
   assert(theGoal.allocated.isZero());
   assert(theGoal.repeat);
   theGoal.target = entry.amount();
   theGoal.targetDates = {m_currentDate, entry.interval()};

   log("Creating category '" + entry.category() + "' for owner '" +
       theCategory.owner + "' to reserve " + theGoal.target.toString() +
       " between dates " +
       theGoal.targetDates.startDate().toString("yyyy-MM-dd") + " and " +
       theGoal.targetDates.endDate().toString("yyyy-MM-dd"));
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
            break;
         case Category::Type::ROUTINE:
            // TODO base this on prior performance in the category
            break;
         case Category::Type::RESERVE_PERCENTAGE:
            break;
         case Category::Type::WITHHOLDING:
            break;
      }
   }

   for (auto& it : m_budget.back().goals)
   {
      allocateGoals(it.first, it.second);
   }
}

// TODO I don't feel great about the way this is written, since most of the
// cases result in a jump to the outer loop, and only one case needs repeating,
// so maybe the inner while loop can be written to only loop when the necessary
// conditions are met
void IPLogger::allocateGoals(std::string const& categoryName, Goals& category)
{
   // for each goal in the category
   for (auto it = category.goals.begin(); it != category.goals.end();
        /*inside*/)
   {
      // allocate as long as the goal has a start date that is within the
      // current period
      while (it->second.targetDates.startDate() <=
             m_budget.back().dates.endDate())
      {
         // do the allocation in the goal
         Currency toAllocate = it->second.target.amortize(
                                  it->second.targetDates,
                                  m_budget.back().dates);
         Currency oldOwnerBalance = m_owners[category.owner];
         moveMoney(it->second.allocated, m_owners[category.owner], toAllocate);
         log("Allocating " + toAllocate.toString() + " for goal '" + it->first +
             "' in category '" + categoryName + "', goal balance is " +
             it->second.allocated.toString() + ", owner '" + category.owner +
             "' previous balance " + oldOwnerBalance.toString() +
             " new balance " + m_owners[category.owner].toString());

         // if we finished the goal, transfer to the category
         if (it->second.targetDates.endDate() <=
             m_budget.back().dates.endDate())
         {
            assert(it->second.allocated == it->second.target);
            Currency oldCatBalance = category.balance;
            moveMoney(category.balance, it->second.allocated,
                      it->second.allocated);
            assert(it->second.allocated.isZero());
            log("Goal '" + it->first + "' reached, transferring to category '" +
                categoryName + "', category balance was " +
                oldCatBalance.toString() + " new balance " +
                category.balance.toString());

            // repeating goals get cleared out and incremented, we will go again
            // if the new start date is within our current period
            if (it->second.repeat)
            {
               it->second.allocated = {};
               ++it->second.targetDates;
            }
            // one-shot goals get erased and we jump to the next goal
            else
            {
               it = category.goals.erase(it);
               // continue in the outer loop, since the inner loop is dangerous
               // if we just erased the last goal
               goto outer;
            }
         }
         // otherwise we will need to allocate more for this goal in the next
         // budget period, so jump out to the next goal
         else
         {
            ++it;
            goto outer;
         }
      }

      // location to jump out to
outer:;
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
