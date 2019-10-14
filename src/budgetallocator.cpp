#include "budgetallocator.h"

// TODO need the ability to configure deposits into the available category, so we can do account setup without needing fake budgets to handle the 'income'

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
      if (!it->isZero() && !it->isNegative())
      {
         qDebug() << "Returning" << it->toString() << "from category" << it.key() << "to available";
      }
      m_available += *it;
   }
   if (m_currentPeriod.startDate() <= QDate::currentDate() && m_currentPeriod.endDate() >= QDate::currentDate())
   {
      m_goalPeriod = m_currentPeriod;
   }
   else {
      m_goalPeriod = DateRange();
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
   // TODO make sure amortize works correctly when faced with null date ranges from empty intersections
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

void BudgetAllocator::processItem(LedgerTransaction const& transaction)
{
   foreach (LedgerTransactionEntry entry, transaction.entries())
   {
      // TODO make sure that account balancer is checking to make sure that on-budget accounts have categories and vice versa
      if (!entry.hasCategory())
      {
         continue;
      }

      if (!m_incomes.contains(entry.category()) && !m_reserves.contains(entry.category()) && !m_routines.contains(entry.category()))
      {
         qDebug() << "Category" << entry.category() << "is unknown and not properly budgeted";
         continue;
      }

      if (m_incomes.contains(entry.category()) && transaction.date() > QDate::currentDate())
      {
         qDebug() << "ignoreing future income for budgeting";
         continue;
      }

      if (m_goals.contains(entry.category()) && !m_goalPeriod.isNull())
      {
         // in the case where we are looking at a goal, and the current budget period already covers the current date, don't try to advance any more
      }
      else {
         advanceBudgetPeriod(transaction.date());

      }

      if (m_goals.contains(entry.category()))
      {
         if (m_goalPeriod.isNull())  // this is a goal that happened in the past
         {
            m_goals[entry.category()] += entry.amount();
            if (m_goals[entry.category()].isNegative())
            {
               qDebug() << "goal category " << entry.category() << "underfunded, compensating";
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
            if ((m_goals[entry.category()] + entry.amount()).isNegative()) /// need to save more for this goal
            {
               // nasty way of getting the positive amount to save
               Currency toSave = Currency() - entry.amount() - m_goals[entry.category()];

               Currency toAllocate =
               toSave.amortize(
               // savings range for goal
               DateRange(m_goalPeriod.startDate(), m_currentPeriod.endDate()),
                        // savings range of budget period that includes today
                        DateRange(m_goalPeriod.startDate(),m_goalPeriod.endDate()));

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
   // give a default monthly period
   if (m_currentPeriod.isNull())
   {
      m_currentPeriod = DateRange(QDate(date.year(), date.month(), 1), Interval(1, Interval::Period::MONTHS));
   }

   if (date < m_currentPeriod.startDate())
   {
      qDebug() << "cannot reset budget period date for out of order item";
      return;
   }

   while (m_currentPeriod.endDate() < date)
   {
      if (m_priorPeriod.isNull())
      {
         m_priorPeriod = m_currentPeriod;
      }
      else {
         m_priorPeriod = DateRange(m_priorPeriod.startDate(), m_currentPeriod.endDate());
      }
      ++m_currentPeriod;
      m_priorRoutine += m_currentRoutine;
      m_currentRoutine.clear();

      if (m_currentPeriod.startDate() <= QDate::currentDate() && m_currentPeriod.endDate() >= QDate::currentDate())
      {
         m_goalPeriod = m_currentPeriod;
      }

      // if we are rebudgeting and the new period starts on the same day as the
      // old one would have, skip that old period
      if (rebudgeting && date == m_currentPeriod.startDate())
      {
         continue;
      }


      for (auto it = m_reserveAmounts.begin(); it != m_reserveAmounts.end(); ++it) {


         // fund all reserve periods that end within this budget period
         // taking into account that the current reserve period might have started before  this budget period, so only do the overlap
         while (m_reservePeriods[it.key()].endDate() <= m_currentPeriod.endDate())
         {
            Currency amount = m_reserveAmounts[it.key()].amortize(m_reservePeriods[it.key()],
                  m_reservePeriods[it.key()].intersect(m_currentPeriod));
            if ((m_available - amount).isNegative())
            {
               qDebug() << "unable to fully fund reserve amount";
               amount = m_available;
            }
            m_reserves[it.key()] += amount;
            m_available -= amount;
            ++m_reservePeriods[it.key()];
         }
         // fund this budget period's share of the next savings period that either starts in this period and ends in a future one, or starts after this period ends
         // TODO make sure amortize works correctly when faced with null date ranges from empty intersections
         Currency amount = m_reserveAmounts[it.key()].amortize(m_reservePeriods[it.key()],
               m_reservePeriods[it.key()].intersect(m_currentPeriod));
         if ((m_available - amount).isNegative())
         {
            qDebug() << "unable to fully fund reserve amount";
            amount = m_available;
         }
         m_reserves[it.key()] += amount;
         m_available -= amount;
      }



      Currency daily =
            m_priorRoutine.amortize(m_priorPeriod, DateRange(m_priorPeriod.startDate(), m_priorPeriod.startDate()));

      daily = daily * m_currentPeriod.days();

      if ((m_available + daily).isNegative())
      {
         qDebug() << "cannot fully fund routine escrow for current budget period";
         m_escrow += m_available;
         m_available.clear();
      }
      else {
         m_available += daily;
         m_escrow -= daily;
      }
   }
}
