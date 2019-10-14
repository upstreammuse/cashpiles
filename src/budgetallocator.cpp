#include "budgetallocator.h"

// TODO need the ability to configure deposits into the available category, so we can do account setup without needing fake budgets to handle the 'income'

#include <QDebug>
#include "ledgerbudget.h"
#include "ledgertransaction.h"
#include "ledgertransactionentry.h"

void BudgetAllocator::finish()
{
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
   refreshCurrentPeriod(budget.date(), true);

   m_currentPeriod = DateRange(budget.date(), budget.interval());
   m_currentRoutine.clear();
   m_incomes.clear();
   m_priorPeriod = DateRange();
   m_priorRoutine.clear();
   m_reservePercentages.clear();
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

void BudgetAllocator::processItem(LedgerBudgetIncomeEntry const& budget)
{
   m_incomes.insert(budget.name());
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
   refreshCurrentPeriod(transaction.date());

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

      if (m_incomes.contains(entry.category()))
      {
         if (transaction.date() > QDate::currentDate())
         {
            qDebug() << "ignoring future income for budget";
            continue;
         }
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

void BudgetAllocator::refreshCurrentPeriod(QDate const& date, bool ignoreIfFirstDay)
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

      if (!ignoreIfFirstDay || date != m_currentPeriod.startDate())
      {

         // TODO need to fund reserve amounts at this time as well


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
}
