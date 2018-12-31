#include "routineallocator.h"

#include "daterange.h"

Currency RoutineAllocator::allocate(Currency available)
{
   for (auto it = m_allocations.begin(); it != m_allocations.end(); ++it)
   {
      if (it->isNegative())
      {
         available += *it;
         it->clear();
      }
   }
   return available;
}

Currency RoutineAllocator::allocate(DateRange const& period,
                                    QString const& category, Currency available)
{
   DateRange range(m_history.cbegin().key(), m_history.cend().key());
   DateRange firstDay(range.startDate(), range.startDate());
   Currency amount = m_historyTotals[category].amortize(range, firstDay) *
                     period.days();
   m_allocations[category] -= amount;
   available += amount;
   return available;
}

Currency RoutineAllocator::deallocate(QString const& category)
{
   Currency result = m_allocations[category];
   m_allocations.remove(category);
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
   return result;
}

bool RoutineAllocator::isUnderfunded() const
{
   foreach (Currency const& c, m_allocations)
   {
      if (c.isNegative())
      {
         return true;
      }
   }
   return false;
}

void RoutineAllocator::spend(QDate const& date, QString const& category,
                             Currency const &amount)
{
   m_history.insertMulti(date, QPair<QString, Currency>(category, amount));
   m_historyTotals[category] += amount;
}
