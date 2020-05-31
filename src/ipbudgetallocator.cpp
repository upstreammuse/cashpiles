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
#include "reportbudget.h"
#include "reporter.h"
#include "texttable.h"

using std::make_shared;
using std::shared_ptr;
using std::string;
using std::stringstream;

IPBudgetAllocator::IPBudgetAllocator(Reporter& reporter) :
   m_reporter(reporter)
{
}

Currency IPBudgetAllocator::budgetable() const
{
   Currency total;
   for (auto it = m_availables.begin(); it != m_availables.end(); ++it)
   {
      total += it->second;
   }
   for (auto it1 : m_goals)
   {
      for (auto it2 : it1.second.goals)
      {
         total += it2.second.reserved;
      }
      total += it1.second.spent;
   }
   for (auto it = m_reserves.begin(); it != m_reserves.end(); ++it)
   {
      total += it->second.reserved;
   }
   for (auto it = m_routines.begin(); it != m_routines.end(); ++it)
   {
      total += it->second.reserved;
   }
   return total;
}

void IPBudgetAllocator::finish()
{
   TextTable table;
   Currency total;
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
         stringstream ss;
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
      stringstream ss;
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
      std::cout << it->first << ": " << it->second.toString() << std::endl;
      if (it->second.isNegative())
      {
         std::cout << "WARNING: over budget!" << std::endl;
      }
   }

   if (m_report)
   {
      generateReport();
   }
}

void IPBudgetAllocator::processItem(LedgerAccount const& account)
{
   advanceBudgetPeriod(account.fileName(), account.lineNum(), account.date());
}

bool IPBudgetAllocator::processItem(LedgerBudget const& budget)
{
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
   // TODO maybe better to track the routine calculation dates per category
   advanceBudgetPeriod(DateRange(budget.date(), budget.interval()));

   // at this point we have reset the budget period to start with the new
   // period's date, and there is nothing more to do, because categories that
   // automatically fund themselves in each period will do that when they are
   // first created
   return true;
}

void IPBudgetAllocator::processItem(LedgerBudgetCancelEntry const& entry)
{
   // TODO remove this
   stringstream ss;
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

void IPBudgetAllocator::processItem(LedgerBudgetCloseEntry const& entry)
{
   auto category = entry.category();

   if (m_goals.count(category))
   {
      for (auto it : m_goals[category].goals)
      {
         m_goals[category].spent += it.second.reserved;
      }

      if (!m_goals[category].spent.isZero())
      {
         stringstream ss;
         ss << "Returning " << m_goals[category].spent.toString()
            << " from category '" << category << "' to available";
         warn(entry.fileName(), entry.lineNum(), ss.str());
      }
      m_availables[m_owners[category]] += m_goals[category].spent;
      m_goals.erase(category);
      m_owners.erase(category);
   }
   else if (m_incomes.count(category))
   {
      m_incomes.erase(category);
      m_owners.erase(category);
   }
   else if (m_reserves.count(category))
   {
      if (!m_reserves[category].reserved.isZero())
      {
         stringstream ss;
         ss << "Returning " << m_reserves[category].reserved.toString()
            << " from category '" << category << "' to available";
         warn(entry.fileName(), entry.lineNum(), ss.str());
      }
      m_availables[m_owners[category]] += m_reserves[category].reserved;
      m_reserves.erase(category);
      m_owners.erase(category);
   }
   else if (m_routines.count(category))
   {
      if (!m_routines[category].reserved.isZero())
      {
         stringstream ss;
         ss << "Returning " << m_routines[category].reserved.toString()
            << " from category '" << category << "' to available";
         warn(entry.fileName(), entry.lineNum(), ss.str());
      }
      m_availables[m_owners[category]] += m_routines[category].reserved;
      m_routines.erase(category);
      m_owners.erase(category);
   }
   else if (m_withholdings.count(category))
   {
      m_withholdings.erase(category);
      m_owners.erase(category);
   }
   else
   {
      stringstream ss;
      ss << "Cannot close budget category '" << category
         << "' that did not already exist";
      warn(entry.fileName(), entry.lineNum(), ss.str());
   }
}

void IPBudgetAllocator::processItem(LedgerBudgetGoalEntry const& entry)
{
   auto category = entry.category();

   if (!m_goals.count(category))
   {
      die(entry.fileName(), entry.lineNum(),
          "Budget goal uses unknown category");
   }
   if (m_goals[category].goals.count(entry.goal()))
   {
      die(entry.fileName(), entry.lineNum(), "Budget goal already in use");
   }

   m_goals[category].goals[entry.goal()].amount = entry.amount();
   m_goals[category].goals[entry.goal()].period =
         DateRange(m_workingDate, entry.goalDate());

   syncGoal(category, entry.goal());
}

void IPBudgetAllocator::processItem(LedgerBudgetGoalsEntry const& entry)
{
   auto category = entry.category();
   auto owner = entry.owner();

   if (m_owners.count(category))
   {
      die(entry.fileName(), entry.lineNum(),
          "Budget category listed multiple times");
   }
   m_availables[owner];
   m_goals[category];
   m_owners[category] = owner;
}

void IPBudgetAllocator::processItem(LedgerBudgetIncomeEntry const& entry)
{
   auto category = entry.category();
   auto owner = entry.owner();

   if (m_owners.count(category))
   {
      die(entry.fileName(), entry.lineNum(),
          "Budget category listed multiple times");
   }
   m_availables[owner];
   m_incomes.insert(category);
   m_owners[category] = owner;
}

void IPBudgetAllocator::processItem(
      LedgerBudgetReserveAmountEntry const& entry)
{
   auto category = entry.category();
   auto owner = entry.owner();

   if (m_owners.count(category))
   {
      die(entry.fileName(), entry.lineNum(),
          "Budget category listed multiple times");
   }

   m_availables[owner];
   m_owners[category] = owner;
   m_reserves[category].amount = entry.amount();
   m_reserves[category].period = DateRange(m_currentPeriod.startDate(),
                                           entry.interval());
   syncReserve(category);
}

void IPBudgetAllocator::processItem(
      LedgerBudgetReservePercentEntry const &entry)
{
   auto category = entry.category();
   auto owner = entry.owner();

   if (m_owners.count(category))
   {
      die(entry.fileName(), entry.lineNum(),
          "Budget category listed multiple times");
   }
   m_availables[owner];
   m_owners[category] = owner;
   m_reserves[category].percentage = entry.percentage() / 100.0;
}

void IPBudgetAllocator::processItem(LedgerBudgetRoutineEntry const& entry)
{
   auto category = entry.category();
   auto owner = entry.owner();

   if (m_owners.count(category))
   {
      die(entry.fileName(), entry.lineNum(),
          "Budget category listed multiple times");
   }
   m_availables[owner];
   m_owners[category] = owner;
   m_routines[category];
}

void IPBudgetAllocator::processItem(LedgerBudgetWithholdingEntry const& entry)
{
   auto category = entry.category();
   auto owner = entry.owner();

   if (m_owners.count(category))
   {
      die(entry.fileName(), entry.lineNum(),
          "Budget category listed multiple times");
   }
   m_availables[owner];
   m_owners[category] = owner;
   m_withholdings.insert(category);
}

bool IPBudgetAllocator::processItem(LedgerTransactionV2 const& transaction)
{
   advanceBudgetPeriod(transaction.fileName(), transaction.lineNum(),
                       transaction.date());
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
         stringstream ss;
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
         stringstream ss;
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
      stringstream ss;
      ss << "Category '" << category << "' not recognized";
      die(entry.fileName(), entry.lineNum(), ss.str());
   }
}

void IPBudgetAllocator::processItem(LedgerTransactionV2OwnerEntry const& entry)
{
   // if the category is an owner, make sure we recognize it
   if (!m_availables.count(entry.owner()))
   {
      stringstream ss;
      ss << "Unknown category owner '" << entry.owner() << "'";
      die(entry.fileName(), entry.lineNum(), ss.str());
   }

   m_availables[entry.owner()] += entry.amount().first;
}

void IPBudgetAllocator::advanceBudgetPeriod(string const& filename,
                                            size_t lineNum, Date const& date)
{
   // use a monthly period by default if not initialized otherwise
   if (m_currentPeriod.isNull())
   {
      warn(filename, lineNum, "Creating a default monthly budget period");
      advanceBudgetPeriod(DateRange(Date(date.year(), date.month(), 1),
                                    Interval(1, Interval::Period::MONTHS)));
   }

   if (date < m_currentPeriod.startDate())
   {
      die(filename, lineNum,
          "Cannot rewind budget period for earlier dated item");
   }

   // stop advancing once the budget period covers the requested date
   while (m_currentPeriod.endDate() < date)
   {
      DateRange nextPeriod = m_currentPeriod;
      ++nextPeriod;
      advanceBudgetPeriod(nextPeriod);
   }
}

void IPBudgetAllocator::advanceBudgetPeriod(DateRange const& period)
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
   m_currentPeriod = period;

   // fund each goals category based on its goals
   // the wonky double-pass is due to possible iterator invalidation when
   // syncGoal completes a goal and removes it from the list
   // TODO there should be a better way to do this that doesn't invalidate
   //   the data as you go
   std::vector<std::pair<string, string>> goals;
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

   if (!m_priorPeriod.isNull())
   {
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

   generateReport();
}

void IPBudgetAllocator::generateReport()
{
   auto report = this->report();
   m_reporter.appendReport(report);
   m_report.reset();
}

shared_ptr<ReportBudget> IPBudgetAllocator::report()
{
   if (!m_report)
   {
      m_report = make_shared<ReportBudget>();
      m_report->setDateRange(m_currentPeriod);
   }
   return m_report;
}

void IPBudgetAllocator::syncGoal(
      string const& category, string const& goal)
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

void IPBudgetAllocator::syncReserve(string const& category)
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
