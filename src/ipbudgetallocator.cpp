#include "ipbudgetallocator.h"

#include <cassert>
#include <iostream>
#include <sstream>
#include "cashpiles.h"
#include "ledgeraccount.h"
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
#include "texttable.h"

IPBudgetAllocator::IPBudgetAllocator(Date const& today) :
   m_today(today)
{
   if (!m_today.isValid())
   {
      std::stringstream ss;
      ss << "Today's date '" << m_today.toString() << "' is invalid";
      die(ss.str());
   }
}

void IPBudgetAllocator::finish()
{
   TextTable table;
   Currency total;
   Currency totalAll;
   table.appendColumn(0, "== CATEGORY ==  ");
   table.appendColumn(1, "== GOAL ==  ");
   table.appendColumn(2, "== AMOUNT ==  ");
   table.appendColumn(3, "== DATE ==");
   table.appendColumn(4, "== RESERVED ==  ");
   for (auto it1 : m_goals)
   {
      for (auto it2 : it1.second.goals)
      {
         table.appendColumn(0, it1.first + "  ");
         table.appendColumn(1, it2.first + "  ");
         table.appendColumn(2, it2.second.amount.toString() + "  ");
         table.appendColumn(3, it2.second.period.endDate().toString() + "  ");
         table.appendColumn(4, it2.second.reserved.toString() + "  ");
         total += it2.second.reserved;
      }
      table.appendColumn(0, it1.first + "  ");
      table.appendColumn(1, "  ");
      table.appendColumn(2, "  ");
      table.appendColumn(3, "  ");
      table.appendColumn(4, it1.second.spent.toString() + "  ");
      total += it1.second.spent;
   }
   totalAll += total;
   table.appendColumn(0, "== TOTAL ==  ");
   table.appendColumn(4, total.toString() + "  ");
   table.setColumnAlignment(2, TextTable::Alignment::RightAlign);
   table.setColumnAlignment(3, TextTable::Alignment::RightAlign);
   table.setColumnAlignment(4, TextTable::Alignment::RightAlign);
   table.print(std::cout);
   std::cout << std::endl;

   table.clear();
   total.clear();
   table.appendColumn(0, "== RESERVE ==  ");
   table.appendColumn(1, "== AMOUNT ==  ");
   table.appendColumn(2, "== PERIOD ==  ");
   table.appendColumn(3, "== PERCENTAGE ==  ");
   table.appendColumn(4, "== AVAILABLE ==");
   for (auto it = m_reserves.begin(); it != m_reserves.end(); ++it)
   {
      table.appendColumn(0, it->first + "  ");
      if (it->second.period.isNull())
      {
         table.appendColumn(1, "  ");
         table.appendColumn(2, "  ");
         std::stringstream ss;
         ss << it->second.percentage << "  ";
         table.appendColumn(3, ss.str());
      }
      else
      {
         table.appendColumn(1, it->second.amount.toString() + "  ");
         table.appendColumn(2, it->second.period.startDate().toString() + "-" +
                            it->second.period.endDate().toString() + "  ");
         table.appendColumn(3, "  ");
      }
      table.appendColumn(4, it->second.reserved.toString());
      total += it->second.reserved;
   }
   totalAll += total;
   table.appendColumn(0, "== TOTAL ==  ");
   table.appendColumn(4, total.toString());
   table.setColumnAlignment(1, TextTable::Alignment::RightAlign);
   table.setColumnAlignment(3, TextTable::Alignment::RightAlign);
   table.setColumnAlignment(4, TextTable::Alignment::RightAlign);
   table.print(std::cout);
   std::cout << std::endl;

   table.clear();
   total.clear();
   table.appendColumn(0, "Routine  ");
   table.appendColumn(1, "History  ");
   table.appendColumn(2, "6-Month  ");
   table.appendColumn(3, "Available");
   std::cout << "Routine history period "
             << m_priorPeriod.startDate().toString()
             << " - " << m_priorPeriod.endDate().toString() << std::endl;
   for (auto it = m_routines.begin(); it != m_routines.end(); ++it)
   {
      std::stringstream ss;
      ss << it->first << "  ";
      table.appendColumn(0, ss.str());
      table.appendColumn(1, it->second.priorAmount.toString() + "  ");
      table.appendColumn(2, (it->second.priorAmount.amortize(
                                m_priorPeriod,
                                DateRange(m_priorPeriod.startDate(),
                                          m_priorPeriod.startDate())) *
                             180LL).toString() + "  ");
      table.appendColumn(3, it->second.reserved.toString());
      total += it->second.reserved;
   }
   totalAll += total;
   table.appendColumn(0, "== TOTAL ==  ");
   table.appendColumn(3, total.toString());
   table.setColumnAlignment(1, TextTable::Alignment::RightAlign);
   table.setColumnAlignment(2, TextTable::Alignment::RightAlign);
   table.setColumnAlignment(3, TextTable::Alignment::RightAlign);
   table.print(std::cout);
   std::cout << std::endl;

   std::cout << "Available to budget: " << std::endl;
   for (auto it = m_availables.begin(); it != m_availables.end(); ++it)
   {
      totalAll += it->second;
      std::cout << it->first << ": " << it->second.toString() << std::endl;
      if (it->second.isNegative())
      {
         std::cout << "WARNING: over budget!" << std::endl;
      }
   }
   std::cout << "Total budgetable money: " << totalAll.toString() << std::endl;
}

void IPBudgetAllocator::processItem(LedgerAccount const& account)
{
   advanceBudgetPeriod(account.fileName(), account.lineNum(), account.date());
}

bool IPBudgetAllocator::processItem(LedgerBudget const& budget)
{
   if (budget.date() > m_today)
   {
      warn(budget.fileName(), budget.lineNum(),
           "Ignoring future budget configuration");
      return false;
   }

   // update the current processing date for entries
   m_workingDate = budget.date();

   // make sure that the budget period is advanced to include yesterday, and
   // that yesterday is the end of the last period
   // in other words make sure that we ended a period yesterday, and that
   // nothing else has yet happened today, such that the budget command is the
   // first item to occur on the first day of a new period
   if (!m_currentPeriod.isNull())
   {
      if (budget.date() <= m_currentPeriod.endDate())
      {
         die(budget.fileName(), budget.lineNum(),
             "Budget can only be reconfigured as the first item in a new "
             "budget period.");
      }
      advanceBudgetPeriod(budget.fileName(), budget.lineNum(),
                          budget.date().addDays(-1));
      if (m_currentPeriod.endDate().addDays(1) != budget.date())
      {
         die(budget.fileName(), budget.lineNum(),
             "Budget can only be reconfigured as the first item in a new "
             "budget period.");
      }
   }

   // reconfigure the budget period
   // TODO this looks like it resets the routine calculations
   m_currentPeriod = DateRange(budget.date(), budget.interval());
   m_priorPeriod = DateRange();
   // TODO maybe better to track the routine calculation dates per category

   // at this point we have reset the budget period to start with the new
   // period's date, and there is nothing more to do, because categories that
   // automatically fund themselves in each period will do that when they are
   // first created
   return true;
}

void IPBudgetAllocator::processItem(LedgerBudgetCancelEntry const& entry)
{
   // TODO remove this
   std::stringstream ss;
   ss << entry.category();
   auto category = ss.str();
   auto goal = entry.goal();

   if (!m_goals.count(category))
   {
      die(entry.fileName(), entry.lineNum(), "Cannot cancel unknown category");
   }
   if (!m_goals[category].goals.count(goal))
   {
      die(entry.fileName(), entry.lineNum(), "Cannot cancel unknown goal");
   }

   m_goals[category].spent += m_goals[category].goals[goal].reserved;
   m_goals[category].goals.erase(goal);
}

void IPBudgetAllocator::processItem(LedgerBudgetCloseEntry const& budget)
{
   // TODO remove this
   std::stringstream ss;
   ss << budget.category();
   auto category = ss.str();

   if (m_goals.count(category))
   {
      for (auto it : m_goals[category].goals)
      {
         m_goals[category].spent += it.second.reserved;
      }

      if (!m_goals[category].spent.isZero())
      {
         std::stringstream ss;
         ss << "Returning " << m_goals[category].spent.toString()
            << " from category '" << category << "' to available";
         warn(budget.fileName(), budget.lineNum(), ss.str());
      }
      m_availables[m_owners[budget.category()]] += m_goals[category].spent;
      m_goals.erase(category);
      m_owners.erase(budget.category());
   }
   else if (m_incomes.find(budget.category()) != m_incomes.end())
   {
      m_incomes.erase(budget.category());
      m_owners.erase(budget.category());
   }
   else if (m_reserves.find(budget.category()) != m_reserves.end())
   {
      if (!m_reserves[budget.category()].reserved.isZero())
      {
         std::stringstream ss;
         ss << "Returning " << m_reserves[budget.category()].reserved.toString()
            << " from category '" << budget.category() << "' to available";
         warn(budget.fileName(), budget.lineNum(), ss.str());
      }
      m_availables[m_owners[budget.category()]] +=
            m_reserves[budget.category()].reserved;
      m_reserves.erase(budget.category());
      m_owners.erase(budget.category());
   }
   else if (m_routines.find(budget.category()) != m_routines.end())
   {
      if (!m_routines[budget.category()].reserved.isZero())
      {
         std::stringstream ss;
         ss << "Returning " << m_routines[budget.category()].reserved.toString()
            << " from category '" << budget.category() << "' to available";
         warn(budget.fileName(), budget.lineNum(), ss.str());
      }
      m_availables[m_owners[budget.category()]] +=
            m_routines[budget.category()].reserved;
      m_routines.erase(budget.category());
      m_owners.erase(budget.category());
   }
   else if (m_withholdings.find(budget.category()) != m_withholdings.end())
   {
      m_withholdings.erase(budget.category());
      m_owners.erase(budget.category());
   }
   else
   {
      std::stringstream ss;
      ss << "Cannot close budget category '" << budget.category()
         << "' that did not already exist";
      warn(budget.fileName(), budget.lineNum(), ss.str());
   }
}

void IPBudgetAllocator::processItem(LedgerBudgetGoalEntry const& budget)
{
   auto category = budget.category();

   if (!m_goals.count(category))
   {
      die(budget.fileName(), budget.lineNum(),
          "Budget goal uses unknown category");
   }
   if (m_goals[category].goals.count(budget.goal()))
   {
      die(budget.fileName(), budget.lineNum(), "Budget goal already in use");
   }

   m_goals[category].goals[budget.goal()].amount = budget.amount();
   m_goals[category].goals[budget.goal()].period =
         DateRange(m_workingDate, budget.goalDate());

   syncGoal(category, budget.goal());
}

void IPBudgetAllocator::processItem(LedgerBudgetGoalsEntry const& budget)
{
   // TODO remove this
   std::stringstream ss;
   ss << budget.category();
   auto category = ss.str();

   if (m_owners.find(budget.category()) != m_owners.end())
   {
      die(budget.fileName(), budget.lineNum(),
          "Budget category listed multiple times");
   }
   m_availables[budget.owner()];
   m_goals[category];
   m_owners[budget.category()] = budget.owner();
}

void IPBudgetAllocator::processItem(LedgerBudgetIncomeEntry const& budget)
{
   if (m_owners.find(budget.category()) != m_owners.end())
   {
      die(budget.fileName(), budget.lineNum(),
          "Budget category listed multiple times");
   }
   m_availables[budget.owner()];
   m_incomes.insert(budget.category());
   m_owners[budget.category()] = budget.owner();
}

void IPBudgetAllocator::processItem(
      LedgerBudgetReserveAmountEntry const& budget)
{
   if (m_owners.find(budget.category()) != m_owners.end())
   {
      die(budget.fileName(), budget.lineNum(),
          "Budget category listed multiple times");
   }

   m_availables[budget.owner()];
   m_owners[budget.category()] = budget.owner();
   m_reserves[budget.category()].amount = budget.amount();
   m_reserves[budget.category()].period = DateRange(m_currentPeriod.startDate(),
                                                    budget.interval());
   syncReserve(budget.category());
}

void IPBudgetAllocator::processItem(
      LedgerBudgetReservePercentEntry const &budget)
{
   if (m_owners.find(budget.category()) != m_owners.end())
   {
      die(budget.fileName(), budget.lineNum(),
          "Budget category listed multiple times");
   }
   m_availables[budget.owner()];
   m_owners[budget.category()] = budget.owner();
   m_reserves[budget.category()].percentage = budget.percentage() / 100.0;
}

void IPBudgetAllocator::processItem(LedgerBudgetRoutineEntry const& budget)
{
   if (m_owners.find(budget.category()) != m_owners.end())
   {
      die(budget.fileName(), budget.lineNum(),
          "Budget category listed multiple times");
   }
   m_availables[budget.owner()];
   m_owners[budget.category()] = budget.owner();
   m_routines[budget.category()];
}

void IPBudgetAllocator::processItem(LedgerBudgetWithholdingEntry const& budget)
{
   if (m_owners.find(budget.category()) != m_owners.end())
   {
      die(budget.fileName(), budget.lineNum(),
          "Budget category listed multiple times");
   }
   m_availables[budget.owner()];
   m_owners[budget.category()] = budget.owner();
   m_withholdings.insert(budget.category());
}

bool IPBudgetAllocator::processItem(LedgerTransactionV2 const& transaction)
{
   advanceBudgetPeriod(transaction.fileName(), transaction.lineNum(),
                       transaction.date());
   // TODO should probably allow as long as it's within the budget period
   if (transaction.date() > m_today) return false;
   m_workingDate = transaction.date();
   return true;
}

void IPBudgetAllocator::processItem(
      LedgerTransactionV2CategoryEntry const& entry)
{
   auto amount = entry.amount().first;
   auto category = entry.category();

   if (m_goals.count(category))
   {
      m_goals[category].spent += amount;
      auto total = m_goals[category].spent;
      for (auto goal : m_goals[category].goals)
      {
         total += goal.second.reserved;
      }
      if (total.isNegative())
      {
         std::stringstream ss;
         ss << "Goal category '" << category << "' overspent";
         warn(entry.fileName(), entry.lineNum(), ss.str());
         m_availables[m_owners[category]] += total;
         m_goals[category].spent -= total;
      }
   }
   else if (m_incomes.count(category))
   {
      m_availables[m_owners[category]] += amount;
      for (auto it = m_reserves.begin(); it != m_reserves.end(); ++it)
      {
         // skip reserves that are not percentage based
         // TODO make separate lists to avoid this crap?
         if (!it->second.period.isNull()) continue;

         // skip reserves with a different owner
         if (m_owners[it->first] != m_owners[category]) continue;

         Currency partial = amount * it->second.percentage;
         it->second.reserved += partial;
         m_availables[m_owners[it->first]] -= partial;
      }
   }
   else if (m_reserves.count(category))
   {
      m_reserves[category].reserved += amount;
      if (m_reserves[category].reserved.isNegative())
      {
         std::stringstream ss;
         ss << "Reserve category '" << category << "' overspent";
         warn(entry.fileName(), entry.lineNum(), ss.str());
         m_availables[m_owners[category]] += m_reserves[category].reserved;
         m_reserves[category].reserved.clear();
      }
   }
   else if (m_routines.count(category))
   {
      m_routines[category].currentAmount += amount;
      m_routines[category].reserved += amount;
      if (m_routines[category].reserved.isNegative())
      {
         // TODO instead of reporting the overspend, this should feed into a
         //   report that says "0 days of routine expenses planned for" or some
         //   such
         m_availables[m_owners[category]] += m_routines[category].reserved;
         m_routines[category].reserved.clear();
      }
   }
   else if (m_withholdings.count(category))
   {
      m_availables[m_owners[category]] += amount;
   }
   else
   {
      std::stringstream ss;
      ss << "Category '" << category << "' not recognized";
      die(entry.fileName(), entry.lineNum(), ss.str());
   }
}

void IPBudgetAllocator::processItem(LedgerTransactionV2OwnerEntry const& entry)
{
   // if the category is an owner, make sure we recognize it
   if (!m_availables.count(entry.owner()))
   {
      std::stringstream ss;
      ss << "Unknown category owner '" << entry.owner() << "'";
      die(entry.fileName(), entry.lineNum(), ss.str());
   }

   m_availables[entry.owner()] += entry.amount().first;
}

void IPBudgetAllocator::advanceBudgetPeriod(std::string const& filename,
                                            size_t lineNum, Date const& date)
{
   // use a monthly period by default if not initialized otherwise
   if (m_currentPeriod.isNull())
   {
      warn(filename, lineNum, "Creating a default monthly budget period");
      m_currentPeriod = DateRange(Date(date.year(), date.month(), 1),
                                  Interval(1, Interval::Period::MONTHS));
   }

   if (date < m_currentPeriod.startDate())
   {
      die(filename, lineNum,
          "Cannot rewind budget period for earlier dated item");
   }

   // stop advancing once the budget period covers either the requested date or
   // the current date, whichever comes first
   while (m_currentPeriod.endDate() < date &&
          m_currentPeriod.endDate() < m_today)
   {
      // merge routine info and reset for the new budget period
      if (m_priorPeriod.isNull())
      {
         m_priorPeriod = m_currentPeriod;
      }
      else
      {
         m_priorPeriod = DateRange(m_priorPeriod.startDate(),
                                   m_currentPeriod.endDate());
      }
      for (auto it = m_routines.begin(); it != m_routines.end(); ++it)
      {
         it->second.priorAmount += it->second.currentAmount;
         it->second.currentAmount.clear();
      }

      // advance the budget period to the new dates
      ++m_currentPeriod;

      // fund each goals category based on its goals
      // the wonky double-pass is due to possible iterator invalidation when
      // syncGoal completes a goal and removes it from the list
      // TODO there should be a better way to do this that doesn't invalidate
      //   the data as you go
      std::vector<std::pair<std::string, std::string>> goals;
      for (auto it1 : m_goals)
      {
         for (auto it2 : it1.second.goals)
         {
            goals.push_back(make_pair(it1.first, it2.first));
         }
      }
      for (auto goal : goals)
      {
         syncGoal(goal.first, goal.second);
      }

      // fund each category that allocates amounts per budget period
      for (auto it = m_reserves.begin(); it != m_reserves.end(); ++it)
      {
         // skip reserves that are not amount/period based
         if (it->second.period.isNull()) continue;

         syncReserve(it->first);
      }

      // fund the routine categories based on prior daily routine expenses and
      // the duration of the current budget period
      for (auto it = m_routines.begin(); it != m_routines.end(); ++it)
      {
         // note that the daily value was a negative number, since it was based
         // on routine expenses
         Currency daily = it->second.priorAmount.amortize(
                             m_priorPeriod,
                             DateRange(m_priorPeriod.startDate(),
                                       m_priorPeriod.startDate())) *
                          m_currentPeriod.days();
         m_availables[m_owners[it->first]] += daily;
         it->second.reserved -= daily;
      }
   }
}

void IPBudgetAllocator::syncGoal(
      std::string const& category, std::string const& goal)
{
   auto& theGoal = m_goals[category].goals[goal];
   auto amount = theGoal.amount.amortize(theGoal.period, m_currentPeriod);
   theGoal.reserved += amount;
   m_availables[m_owners[category]] -= amount;
   if (theGoal.period.endDate() <= m_currentPeriod.endDate())
   {
      assert(theGoal.reserved == theGoal.amount);
      m_goals[category].spent += theGoal.reserved;
      m_goals[category].goals.erase(goal);
   }
}

void IPBudgetAllocator::syncReserve(std::string const& category)
{
   // fund all reserve periods that start before this budget period ends,
   // considering that the current reserve period might have started
   // before this budget period, so only do the overlap
   while (m_reserves[category].period.startDate() <= m_currentPeriod.endDate())
   {
      // since the savings period and the budget period can be different,
      // we only want to allocate the amount that represents the overlap
      // between the savings period and the budget period
      Currency amount = m_reserves[category].amount.amortize(
                           m_reserves[category].period, m_currentPeriod);

      // move funds from available to savings goal
      m_reserves[category].reserved += amount;
      m_availables[m_owners[category]] -= amount;

      // if the reserve period extends into the next budget period, we
      // have to stop without incrementing it, so the next budget period
      // can handle the remainder of this reserve period
      if (m_reserves[category].period.endDate() > m_currentPeriod.endDate())
      {
         break;
      }
      // otherwise increment the reserve period and let the loop run again
      else
      {
         ++m_reserves[category].period;
      }
   }
}
