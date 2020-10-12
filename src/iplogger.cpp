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
#include "ledgertransaction.h"
#include "ledgertransactionv2.h"

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

void IPLogger::processItem(LedgerTransaction const& transaction)
{
   savePosition(transaction);
   processDate(transaction.date());

   if (!m_accounts.count(transaction.account()))
   {
      warn("Transaction uses unknown account '" + transaction.account() + "'");
      return;
   }
   auto& account = m_accounts[transaction.account()];

   if (account.onBudget)
   {
      warn("Single-entry transactions cannot be used with on-budget accounts");
      return;
   }

   // TODO need to mark the account as uncleared if we get an uncleared
   // transaction, so that the balance check can warn that a balance includes
   // uncleared items

   auto oldBalance = account.balance;
   account.balance += transaction.amount();

   log(transaction.date().toString("yyyy-MM-dd") + ": transaction for payee '" +
       transaction.payee() + "' account '" + transaction.account() +
       "' old balance " + oldBalance.toString() + " new balance " +
       account.balance.toString());
}

bool IPLogger::processItem(LedgerTransactionV2 const& transaction)
{
   savePosition(transaction);
   processDate(transaction.date());

   // TODO cache the status so we have access to it for each entry in the txn

   log(transaction.date().toString("yyyy-MM-dd") + ": transaction for payee '" +
       transaction.payee() + " affects budget by " +
       transaction.amount().toString());
   return true;
}

void IPLogger::processItem(LedgerTransactionV2AccountEntry const& entry)
{
   savePosition(entry);

   if (!m_accounts.count(entry.account()))
   {
      warn("Transaction uses unknown account '" + entry.account() + "'");
      return;
   }
   auto& account = m_accounts[entry.account()];

   if (!account.onBudget)
   {
      warn("Off-budget account '" + entry.account() +
           "' needs to be used with a category");
      return;
   }

   // by the time we get here, all entries should be populated with amounts
   assert(entry.amount().second);
   auto oldBalance = account.balance;
   account.balance += entry.amount().first;
   log("Account '" + entry.account() + "' balance was " +
       oldBalance.toString() + ", updated by " +
       entry.amount().first.toString() + ", new balance is " +
       account.balance.toString());
}

// TODO generally, should we make sure that all categories are non-negative when
// a budget period rolls over?  It's easy to allocate from the owner for each
// category to keep it non-negative.  But is it better to leave negative category
// balances in the budget reports so that the user can more easily see where they
// went wrong?  But then again, does a user want to see this?  Seems like you
// wouldn't want to have to make a bunch of goals for random expenses in the
// past.  We want to give the user the best possible view into the future (and
// safest possible view), but we don't want to nag and penalize for what has
// already happened.  Then again, for a purist like me, the user may want to
// audit the past and create those goals?  Or is it better to conceptualize
// goals as simply a planning tool, with no implied penalty for guessing wrong
// so long as your balance is non-zero?  All the other allocations and spending
// generally take care of themselves, so why make goals be obnoxious?  What's
// done is done.  I think the in-between is to show in a budget report that a
// category was negative at the end of the period, but don't make it a warning
// from the program.  Or maybe don't make it an item in the budget report, but
// make it an optional diagnostic for the sake of users that want to know every
// time they guessed wrong, or planned wrong, but otherwise CP just does its
// business and keeps the data correct.
// - consider that first-time users may well be in debt and spending beyond
//   their means, which obviously needs to be corrected, but a top-level warning
//   that the overall balance is negative should be sufficient to resolve this
// - also, in a debt payoff scenario, the debt account could be made offbudget
//   with the debt repayment handled as a savings goal? (havent thought this
//   through all the way)
void IPLogger::processItem(LedgerTransactionV2CategoryEntry const& entry)
{
   savePosition(entry);

   assert(!m_budget.empty());
   assert(m_budget.back().dates.startDate() <= m_currentDate &&
          m_currentDate <= m_budget.back().dates.endDate());
   assert(entry.amount().second);

   if (entry.trackingAccount().second)
   {
      if (!m_accounts.count(entry.trackingAccount().first))
      {
         warn("Transaction uses unknown account '" +
              entry.trackingAccount().first + "'");
         return;
      }
      auto& account = m_accounts[entry.trackingAccount().first];
      if (account.onBudget)
      {
         warn("Transaction cannot use on-budget account with a category");
         return;
      }
   }

   if (m_budget.back().categories.count(entry.category()))
   {
      auto& category = m_budget.back().categories[entry.category()];
      switch (category.type)
      {
         case Category::Type::INCOME:
            for (auto& categoryToCheck : m_budget.back().categories)
            {
               // TODO make this in if-not-continue check to reduce indent
               if (categoryToCheck.second.type ==
                   Category::Type::RESERVE_PERCENTAGE)
               {
                  auto oldCatBalance = categoryToCheck.second.allocated +
                                       categoryToCheck.second.spent;
                  auto toAllocate = entry.amount().first *
                                    categoryToCheck.second.percentage;

                  // TODO enforce the same-owner rule more clearly
                  auto& owner = m_owners[categoryToCheck.second.owner];
                  if (categoryToCheck.second.owner != category.owner)
                  {
                     continue;
                  }

                  auto oldOwnerBalance = owner;
                  moveMoney(
                           categoryToCheck.second.allocated, owner, toAllocate);

                  stringstream ss;
                  ss << "Reserving " << categoryToCheck.second.percentage
                     << " of income amount " << entry.amount().first.toString()
                     << " to category '" << categoryToCheck.first
                     << "', old balance " << oldCatBalance.toString()
                     << ", new balance "
                     << (categoryToCheck.second.allocated +
                         categoryToCheck.second.spent).toString()
                     << ", old owner balance " << oldOwnerBalance.toString()
                     << ", new owner balance " << owner.toString();
                  log(ss.str());
               }
            }
            break;
         case Category::Type::ROUTINE:
         {
            // TODO since we won't autocorrect the routine category balance
            // here, we need to make sure it's >= 0 when we allocate a routine
            // category at the start of a budget period.  That will keep things
            // deterministic, because allocation is always the very first thing
            // in a budget period, and transactions will either occur before the
            // period, or during/after, so transaction order won't matter

            auto oldCatBalance = category.allocated + category.spent;
            category.spent += entry.amount().first;
            log("Routine category '" + entry.category() + "' old balance " +
                oldCatBalance.toString() + ", new balance " +
                (category.spent + category.allocated).toString());
            break;
         }
         case Category::Type::WITHHOLDING:
         {
            auto& owner = m_owners[category.owner];
            auto oldOwnerBalance = owner;
            owner += entry.amount().first;
            log("Withholding category '" + entry.category() +
                "' applied, old owner balance " + oldOwnerBalance.toString() +
                ", new owner balance" + owner.toString());
            break;
         }
         case Category::Type::RESERVE_PERCENTAGE:
         {
            auto oldCatBalance = category.allocated + category.spent;
            category.spent += entry.amount().first;
            log("Reserve category '" + entry.category() + "' old balance " +
                oldCatBalance.toString() + ", new balance " +
                (category.spent + category.allocated).toString());
            break;
         }
      }
   }
   else if (m_budget.back().goals.count(entry.category()))
   {
      auto& category = m_budget.back().goals[entry.category()];

      auto oldCatBalance = category.balance;
      for (auto goal : category.goals)
      {
         oldCatBalance += goal.second.allocated;
      }

      category.balance += entry.amount().first;

      auto catBalance = category.balance;
      for (auto goal : category.goals)
      {
         catBalance += goal.second.allocated;
      }

      log("Goals category '" + entry.category() + "' old balance " +
          oldCatBalance.toString() + ", new balance " + catBalance.toString());
   }
   else
   {
      warn("Transaction uses unknown category '" + entry.category() + "'");
      return;
   }

   if (entry.trackingAccount().second)
   {
      auto& account = m_accounts[entry.trackingAccount().first];
      auto oldBalance = account.balance;
      // we subtract because we are actually performing the 'other side' of a
      // transfer into the off-budget account
      account.balance -= entry.amount().first;
      log("Tracking account '" + entry.trackingAccount().first +
          "' old balance " + oldBalance.toString() + ", new balance " +
          account.balance.toString());
   }
}

void IPLogger::processItem(LedgerTransactionV2OwnerEntry const& entry)
{
   savePosition(entry);

   assert(entry.amount().second);

   // TODO dry...ish
   if (entry.trackingAccount().second)
   {
      if (!m_accounts.count(entry.trackingAccount().first))
      {
         warn("Transaction uses unknown account '" +
              entry.trackingAccount().first + "'");
         return;
      }
      auto& account = m_accounts[entry.trackingAccount().first];

      if (account.onBudget)
      {
         warn("Transaction cannot use on-budget account with a category");
         return;
      }

      auto oldBalance = account.balance;
      // we subtract because we are actually performing the 'other side' of a
      // transfer into the off-budget account
      // TODO this could in theory be done with moveMoney vs the owner, but not
      //   sure how to express such a thing cleanly since the owner needs to be
      //   updated whether a tracking account is present or not
      account.balance -= entry.amount().first;
      log("Tracking account '" + entry.trackingAccount().first +
          "' old balance " + oldBalance.toString() + ", new balance " +
          account.balance.toString());
   }

   auto& owner = m_owners[entry.owner()];
   auto oldOwnerBalance = owner;
   owner += entry.amount().first;
   log("Owner '" + entry.owner() + "' prior balance " +
       oldOwnerBalance.toString() + ", current balance " + owner.toString());
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

/*
 * CONDITION 1
 *
 * Goal finished before current period
 *
 * goal:   |--------|
 * period:           |--------|
 *
 * This is an error and should never be in this state.  A goal always starts at
 * the same time as a period, so always ensure that a goal overlaps a period by
 * at least by one day.
 *
 *
 * CONDITION 2
 *
 * Goal finishes during current period.  The goal may have started before the
 * current period, it may start coincident with the current period, or it may
 * start within the current period.  The goal may complete on any day of the
 * period, including the first day and the last day.
 *
 * goal:   |------|
 * period:        |-------|
 *
 * goal:   |---------|
 * period:        |-------|
 *
 * goal:   |--------------|
 * period:        |-------|
 *
 * goal:          |-----|
 * period:        |-------|
 *
 * goal:          |-------|
 * period:        |-------|
 *
 * goal:             |--|
 * period:        |-------|
 *
 * goal:             |----|
 * period:        |-------|
 *
 * Allocate based on the overlap of the goal and the period.  Assert that the
 * goal amount has been reached.  If the goal is repeating, then increment the
 * goal dates.  Continue allocating as long as the goal ends during the current
 * period.
 *
 *
 * CONDITION 3
 *
 * Goal finishes after current period.  The goal may have started before the
 * current period, it may start coincident with the current period, or it may
 * start within the current period.
 *
 * goal:   |---------------|
 * period:   |---------|
 *
 * goal:   |---------------|
 * period: |---------|
 *
 * goal:      |---------------|
 * period: |---------|
 *
 * Allocate based on the overlap of the goal and the period.  Nothing more to do
 * at this point.
 */
template<typename T>
auto IPLogger::allocateGoal(
      string const& categoryName, Goals& category,
      string const& goalName, Goal& goal, T& it)
{
   // make sure we didn't miss a previous goal period, by making sure that
   // the goal end is at least current with the current period
   assert(m_budget.back().dates.startDate() <= goal.targetDates.endDate());

   // make sure there's at least something to do
   assert(goal.targetDates.startDate() <= m_budget.back().dates.endDate());

   // allocate as long as the goal starts before the current period ends
   while (goal.targetDates.startDate() <= m_budget.back().dates.endDate())
   {
      // do the allocation in the goal
      Currency toAllocate = goal.target.amortize(goal.targetDates,
                                                 m_budget.back().dates);
      Currency oldOwnerBalance = m_owners[category.owner];
      moveMoney(goal.allocated, m_owners[category.owner], toAllocate);
      log("Allocating " + toAllocate.toString() + " for goal '" + goalName +
          "' in category '" + categoryName + "', goal balance is " +
          goal.allocated.toString() + ", owner '" + category.owner +
          "' previous balance " + oldOwnerBalance.toString() +
          " new balance " + m_owners[category.owner].toString());

      // if we finished the goal, transfer to the category
      if (goal.targetDates.endDate() <=
          m_budget.back().dates.endDate())
      {
         assert(goal.allocated == goal.target);
         Currency oldCatBalance = category.balance;
         moveMoney(category.balance, goal.allocated, goal.allocated);
         assert(goal.allocated.isZero());
         log("Goal '" + goalName + "' reached, transferring to category '" +
             categoryName + "', category balance was " +
             oldCatBalance.toString() + " new balance " +
             category.balance.toString());

         // repeating goals get cleared out and incremented, we will go again
         // if the new start date is within our current period
         if (goal.repeat)
         {
            goal.allocated = {};
            ++goal.targetDates;
         }
         // one-shot goals get erased and we are done
         else
         {
            it = category.goals.erase(it);
            return it;
         }
      }
      // otherwise we will need to allocate more for this goal in the next
      // budget period, so we are done for now
      else
      {
         ++it;
         return it;
      }
   }

   // if we get here we have a goal that starts after the current budget period,
   // so nothing left to do for now
   ++it;
   return it;
}

void IPLogger::allocateGoals(std::string const& categoryName, Goals& category)
{
   // for each goal in the category
   for (auto it = category.goals.begin(); it != category.goals.end();
        /*inside*/)
   {
      it = allocateGoal(categoryName, category, it->first, it->second, it);
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
