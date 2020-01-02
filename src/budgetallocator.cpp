#include "budgetallocator.h"

// TODO need the ability to configure deposits into the available category, so we can do account setup without needing fake budgets to handle the 'income'

// TODO goals need to be saved per transaction?
   // yes, otherwise allocation never looks like enough
    // well, the total allocation for the current budget period needs to be kept sqeparate so that it knows that the saving has alreayd been completed

// as of today, I have $500
//  next week I need $600
//  next month I need $300
// so I must save $100 in the coming week
//  and I must save $75 in the coming week

// 600 + 300 - 500 = 400 to save, but can't do this evenly or will miss soon large amounts

// so for each goal expense (ignore income?), take away the amount already saved, then amortize over the coming periods
//   most expenses will hoave nothing to take away, so will require saving full amount
//   as goals become past expenses, actual values will update required saving amounts


#include <QDebug>
#include "ledgerbudget.h"
#include "ledgertransaction.h"
#include "ledgertransactionentry.h"

void BudgetAllocator::finish()
{
   Currency goal;
   for (auto it = m_goals.begin(); it != m_goals.end(); ++it)
   {
      goal += *it;
   }
   qDebug() << "reserved for goals" << goal.toString();

   Currency reserved;
   for (auto it = m_reserves.begin(); it != m_reserves.end(); ++it)
   {
      reserved += *it;
   }
   qDebug() << "reserved amounts" << reserved.toString();
   qDebug() << "routine expenses" << m_priorRoutine.toString();

   if (!m_priorPeriod.isNull())
   {
      qDebug() << "Routine escrow based on" << m_priorPeriod.startDate().toString() << "to" << m_priorPeriod.endDate().toString();
      qDebug() << "routine escrow balance" << m_escrow.toString();
      Currency daily = m_priorRoutine.amortize(m_priorPeriod, DateRange(m_priorPeriod.startDate(),m_priorPeriod.startDate()));
      qDebug() << "180 days is" << (daily * qint64(180)).toString();

      Currency difference = daily * qint64(180) - m_escrow;
      if (!difference.isZero() && !difference.isNegative())
      {
         qDebug() << "add" << difference.toString() << "to achieve 180 days";
      }
   }

   qDebug() << "remaining available" << m_available.toString();
}

void BudgetAllocator::processItem(LedgerBudget const& budget)
{
   if (budget.date() > QDate::currentDate())
   {
      // TODO trickle dates into budget items so they can be ignored as a group
      qDebug() << "future budget configuration is going to result it weirdness";
   }

   advanceBudgetPeriod(budget.date(), true);

   m_currentPeriod = DateRange(budget.date(), budget.interval());
   m_currentRoutine.clear();
   for (auto it = m_goals.cbegin(); it != m_goals.cend(); ++it)
   {
      // TODO we should also return negative values, or else we will make up money
      if (!it->isZero() && !it->isNegative())
      {
         qDebug() << "Returning" << it->toString() << "from category" << it.key() << "to available";
      }
      m_available += *it;
   }
   m_goals.clear();
   m_incomes.clear();
   m_priorPeriod = DateRange();
   m_priorRoutine.clear();
   m_reserveAmounts.clear();
   m_reservePercentages.clear();
   m_reservePeriods.clear();
   for (auto it = m_reserves.cbegin(); it != m_reserves.cend(); ++it)
   {
      // TODO we should also return negative values, or else we will make up money
      if (!it->isZero() && !it->isNegative())
      {
         qDebug() << "Returning" << it->toString() << "from category" << it.key() << "to available";
      }
      m_available += *it;
   }
   m_reserves.clear();
   m_routines.clear();
}

void BudgetAllocator::processItem(LedgerBudgetGoalEntry const& budget)
{
   m_goals[budget.name()];
}

void BudgetAllocator::processItem(LedgerBudgetIncomeEntry const& budget)
{
   m_incomes.insert(budget.name());
}

void BudgetAllocator::processItem(LedgerBudgetReserveAmountEntry const& budget)
{
   m_reserveAmounts[budget.name()] = budget.amount();
   m_reservePeriods[budget.name()] = DateRange(m_currentPeriod.startDate(), budget.interval());
   m_reserves[budget.name()];

   // fund all reserve periods that end within this budget period
   while (m_reservePeriods[budget.name()].endDate() <= m_currentPeriod.endDate())
   {
      Currency amount = m_reserveAmounts[budget.name()];
      if ((m_available - amount).isNegative())
      {
         qDebug() << "unable to fully fund reserve amount";
         amount = m_available;
      }
      m_reserves[budget.name()] += amount;
      m_available -= amount;
      ++m_reservePeriods[budget.name()];
   }
   // fund this budget period's share of the next savings period that either starts in this period and ends in a future one, or starts after this period ends
   Currency amount = m_reserveAmounts[budget.name()].amortize(m_reservePeriods[budget.name()],
         m_reservePeriods[budget.name()].intersect(m_currentPeriod));
   if ((m_available - amount).isNegative())
   {
      qDebug() << "unable to fully fund reserve amount";
      amount = m_available;
   }
   m_reserves[budget.name()] += amount;
   m_available -= amount;
}

void BudgetAllocator::processItem(LedgerBudgetReservePercentEntry const &budget)
{
   m_reservePercentages[budget.name()] = budget.percentage() / 100.0;
   m_reserves[budget.name()];
}

void BudgetAllocator::processItem(LedgerBudgetRoutineEntry const& budget)
{
   m_routines.insert(budget.name());
}

// TODO need an option to override the apps concept of "today" so that we can check future scenarios if desired
void BudgetAllocator::processItem(LedgerTransaction const& transaction)
{
   // update the current budget period to include the transaction date, but
   // don't go past the budget period that includes the current date
   advanceBudgetPeriod(transaction.date());

   foreach (LedgerTransactionEntry entry, transaction.entries())
   {
      // TODO make sure that account balancer is checking to make sure that on-budget accounts have categories and vice versa
      //   just checked, it isn't doing this yet
      if (!entry.hasCategory())
      {
         continue;
      }

      // TODO is it better to ignore the entry, throwing off our answers, or is it better to create a new category as a routine expense and move on?
      //   this is similar to the problem we face when coming across unknown accounts
      if (!m_incomes.contains(entry.category()) && !m_reserves.contains(entry.category()) && !m_routines.contains(entry.category()))
      {
         qDebug() << "Category" << entry.category() << "is unknown and not properly budgeted";
         continue;
      }

      // ignore future incomes
      if (m_incomes.contains(entry.category()) && transaction.date() > QDate::currentDate())
      {
         qDebug() << "ignoring future income for budgeting";
         continue;
      }

      // ignore non-goal transactions after the current budget period, since the budget period stops with teh current date
      if (!m_goals.contains(entry.category()) && transaction.date() > m_currentPeriod.endDate())
      {
         qDebug() << "ignoring future transaction beyond current budget period";
         continue;
      }

      if (m_goals.contains(entry.category()))
      {
         // TODO it might be possible to replace "goal period" variable with a dynamic check for "current period includes today", since we don't go past current day's period
         if (transaction.date() <= QDate::currentDate())  // this is a goal that happened in the past
         {
            m_goals[entry.category()] += entry.amount();
            if (m_goals[entry.category()].isNegative())
            {
               // TODO change this message, this is a case where a goal wan't saved for properly
               qDebug() << "goal category " << entry.category() << "underfunded, compensating";
               // TODO make generic function to allocate from available to a category, it gets used everywhere
               if ((m_available + m_goals[entry.category()]).isNegative())
               {
                  qDebug() << "failing to allocate for category" << entry.category();
                  m_goals[entry.category()] += m_available;
                  m_available.clear();
               }
               else {
                  // TODO I think this sign is wrong in the code elsewhere
                  m_available += m_goals[entry.category()];
                  m_goals[entry.category()].clear();
               }

            }
         }
         else {
            // in this section, the transaction date is after today, so the current budget period has to include the current date (since we stop at the current date period)
            if ((m_goals[entry.category()] + entry.amount()).isNegative()) /// need to save more for this goal
            {
               // nasty way of getting the positive amount to save
               Currency toSave = Currency() - entry.amount() - m_goals[entry.category()];

               // find out the total date range left to save in, including the current period dates
               DateRange goalPeriod = m_currentPeriod;
               while (goalPeriod.endDate() < transaction.date())
               {
                  ++goalPeriod;
               }
               goalPeriod = DateRange(m_currentPeriod.startDate(), goalPeriod.endDate());

               // savings range for goal split to cover current period days
               Currency toAllocate = toSave.amortize(goalPeriod, m_currentPeriod);

               qDebug() << "need to reserve " << toAllocate.toString() << "in today's budget period to stay on track";
               m_goals[entry.category()].clear();

            }
            else {
               m_goals[entry.category()] += entry.amount();
            }


         }
      }
      else if (m_incomes.contains(entry.category()))
      {
         m_available += entry.amount();
         if (m_available.isNegative())
         {
            qDebug() << "income brought available funds negative";
         }
         for (auto it = m_reservePercentages.begin(); it != m_reservePercentages.end(); ++it)
         {
            Currency amount = entry.amount() * it.value();
            if ((m_available - amount).isNegative())
            {
               qDebug() << "failing to allocate for category" << it.key();
               amount = m_available;
            }
            m_reserves[it.key()] += amount;
            m_available -= amount;
         }
      }
      else if (m_reserves.contains(entry.category()))
      {
         m_reserves[entry.category()] += entry.amount();
         if (m_reserves[entry.category()].isNegative())
         {
            qDebug() << "category" << entry.category() << "overspent, compensating";
            if ((m_available + m_reserves[entry.category()]).isNegative())
            {
               qDebug() << "failing to allocate for category" << entry.category();
               m_reserves[entry.category()] += m_available;
               m_available.clear();
            }
            else {
               m_available -= m_reserves[entry.category()];
               m_reserves[entry.category()].clear();
            }
         }
      }
      else if (m_routines.contains(entry.category()))
      {
         m_currentRoutine += entry.amount();
         m_escrow += entry.amount();
         if (m_escrow.isNegative())
         {
            qDebug() << "routine escrow overspent, compensating";
            if ((m_available + m_escrow).isNegative())
            {
               qDebug() << "failing to allocate for routine escrow";
               m_escrow += m_available;
               m_available.clear();
            }
            else {
               m_available -= m_escrow;
               m_escrow.clear();
            }
         }
      }
      else
      {
         qDebug() << "category is unknown by this allocator";
         // TODO this is a funk state since all the types should be handled above
      }
   }
}

void BudgetAllocator::advanceBudgetPeriod(QDate const& date, bool rebudgeting)
{
   // use a monthly period by default if not initialized otherwise
   if (m_currentPeriod.isNull())
   {
      qDebug() << "using default monthly budget period";
      m_currentPeriod = DateRange(QDate(date.year(), date.month(), 1),
                                  Interval(1, Interval::Period::MONTHS));
   }

   if (date < m_currentPeriod.startDate())
   {
      qDebug() << "cannot rewind budget period for earlier dated item";
      return;
   }

   // stop advancing once the budget period covers either the requested date or
   // the current date, whichever comes first
   while (m_currentPeriod.endDate() < date &&
          m_currentPeriod.endDate() < QDate::currentDate())
   {
      // merge escrow info and reset for the new budget period
      if (m_priorPeriod.isNull())
      {
         m_priorPeriod = m_currentPeriod;
      }
      else
      {
         m_priorPeriod = DateRange(m_priorPeriod.startDate(),
                                   m_currentPeriod.endDate());
      }
      m_priorRoutine += m_currentRoutine;
      m_currentRoutine.clear();

      // advance the budget period to the new dates
      ++m_currentPeriod;

      // if we are rebudgeting and the new period starts on the same day as the
      // old one would have, skip allocating funds for that old period
      if (rebudgeting && date == m_currentPeriod.startDate())
      {
         continue;
      }

      // fund each category that allocates amounts per budget period
      for (auto it = m_reserveAmounts.begin(); it != m_reserveAmounts.end();
           ++it)
      {
         // fund all reserve periods that end within this budget period, taking
         // into account that the current reserve period might have started
         // before this budget period, so only do the overlap
         while (m_reservePeriods[it.key()].endDate() <=
                m_currentPeriod.endDate())
         {
            // since the savings period and the budget period can be different,
            // we only want to allocate the amount that represents the overlap
            // between the savings period and the budget period
            Currency amount = m_reserveAmounts[it.key()].amortize(
                                 m_reservePeriods[it.key()],
                                 m_reservePeriods[it.key()].intersect(
                                       m_currentPeriod));

            // if we can't fund it all, take what we can get
            if ((m_available - amount).isNegative())
            {
               qDebug() << "unable to fully fund reserve amount";
               amount = m_available;
            }

            // move funds from available to savings goal
            m_reserves[it.key()] += amount;
            m_available -= amount;

            // move to the next savings period
            ++m_reservePeriods[it.key()];
         }

         // fund this budget period's share of the next savings period that
         // either starts in this period and ends in a future one, or starts
         // after this period ends
         Currency amount = m_reserveAmounts[it.key()].amortize(
                              m_reservePeriods[it.key()],
                              m_reservePeriods[it.key()].intersect(
                                    m_currentPeriod));

         // if we can't fund it all, take what we can get
         if ((m_available - amount).isNegative())
         {
            qDebug() << "unable to fully fund reserve amount";
            amount = m_available;
         }

         // move funds
         m_reserves[it.key()] += amount;
         m_available -= amount;
      }

      // fund the routine escrow account based on prior daily routine expenses
      // and the duration of the current budget period
      // note that the daily value is a negative number, since it is based on
      // routine expenses
      Currency daily = m_priorRoutine.amortize(
                          m_priorPeriod,
                          DateRange(m_priorPeriod.startDate(),
                                    m_priorPeriod.startDate())) *
                       m_currentPeriod.days();
      // if it is too much, take what we can
      if ((m_available + daily).isNegative())
      {
         qDebug() << "cannot fully fund routine escrow";
         m_escrow += m_available;
         m_available.clear();
      }
      else
      {
         m_available += daily;
         m_escrow -= daily;
      }
   }
}