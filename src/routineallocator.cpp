#include "routineallocator.h"

#include "daterange.h"

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
