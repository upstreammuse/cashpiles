#include "routineallocator.h"

#include <QDebug>
#include "daterange.h"
#include "ledgeraccount.h"
#include "ledgerbudget.h"
#include "ledgertransaction.h"

void RoutineAllocator::finish()
{
   DateRange history(m_startDate, m_endDate);
   qDebug() << "tracking routine expenses between" << m_startDate << "and" << m_endDate;
   qDebug() << "total expenses are" << m_totalSpent.toString();
   qDebug() << "reserving for next 60 days";
   Currency dailyAmount = m_totalSpent.amortize(history, DateRange(m_startDate, m_startDate));
   qDebug() << "daily amount" << dailyAmount.toString();
   qDebug() << (dailyAmount * qint64(60)).toString();
}

void RoutineAllocator::processItem(const LedgerAccount &)
{
}

void RoutineAllocator::processItem(const LedgerBudget& budget)
{
   qDebug() << "starting budget on " << budget.date();
   m_startDate = budget.date();
//   if (m_historyPeriod.isNull())
//   {
//      m_budgetDate = budget.date();
//      m_currentPeriod = DateRange(budget.date(), budget.interval());
//   }/
#if 0
   else
   {
      if (budget.date() < m_currentPeriod.startDate)
      {
         qDebug() << "out of order";
         return;
      }
      while (!m_currentPeriod.includes(budget.date()))
      {
         ++m_currentPeriod;
         Currency escrowAmount =
               m_totalSpent /
               DateRange(m_budgetStartDate, m_dateRange.endDate).daysInPeriod();
         m_available += escrowAmount;
         // TODO allocate during these periods
      }

   }
#endif
}

void RoutineAllocator::processItem(const LedgerBudgetGoalEntry &)
{
}

void RoutineAllocator::processItem(const LedgerBudgetIncomeEntry &)
{

}

void RoutineAllocator::processItem(const LedgerBudgetReserveAmountEntry &)
{
}

void RoutineAllocator::processItem(const LedgerBudgetReservePercentEntry &)
{

}

void RoutineAllocator::processItem(const LedgerBudgetRoutineEntry &budget)
{
   qDebug() << "category" << budget.name() << "is routine";
   m_categories.insert(budget.name());

}

void RoutineAllocator::processItem(const LedgerComment &)
{

}

void RoutineAllocator::processItem(const LedgerTransaction &transaction)
{
   m_endDate = transaction.date();

   foreach (LedgerTransactionEntry entry, transaction.entries())
   {
      if (entry.hasCategory() && m_categories.contains(entry.category()))
      {
         qDebug() << "allocating for category" << entry.category();
         m_totalSpent += entry.amount();
         qDebug() << "total spent" << m_totalSpent.toString();
      }

   }

}

void RoutineAllocator::start()
{

   // DAH FUQ?
   m_totalSpent = Currency();
}


#if 0
Currency RoutineAllocator::allocate(Currency available)
{
   if (m_allocation.isNegative())
   {
      available += m_allocation;
      m_allocation.clear();
   }
   return available;
}

Currency RoutineAllocator::allocate(DateRange const& period, Currency available)
{
   DateRange range;
   if (m_history.isEmpty())
   {
      range = DateRange();
   }
   else
   {
      Q_ASSERT(m_history.lastKey() < period.startDate());
      range = DateRange(m_history.firstKey(), period.startDate().addDays(-1));
   }
   DateRange firstDay(range.startDate(), range.startDate());

   foreach (Currency const& total, m_historyTotals)
   {
      Currency amount = total.amortize(range, firstDay) * period.days();
      m_allocation -= amount;
      available += amount;
   }

   if (available.isNegative())
   {
      m_allocation += available;
      available.clear();
   }
   return available;
}

void RoutineAllocator::deallocate(QString const& category)
{
   for (auto it = m_history.begin(); it != m_history.end(); /*inside*/)
   {
      if (it->first == category)
      {
         it = m_history.erase(it);
      }
      else
      {
         ++it;
      }
   }
   m_historyTotals.remove(category);
}

bool RoutineAllocator::isUnderfunded() const
{
   return m_allocation.isNegative();
}

void RoutineAllocator::spend(QDate const& date, QString const& category,
                             Currency const &amount)
{
   m_history.insertMulti(date, qMakePair(category, amount));
   m_historyTotals[category] += amount;
}
#endif
