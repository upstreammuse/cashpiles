#include "goalallocator.h"

#include "daterange.h"

Currency GoalAllocator::allocate(Currency available)
{
   if (m_allocation.isNegative())
   {
      available += m_allocation;
      m_allocation.clear();
   }
   return available;
}

Currency GoalAllocator::allocate(DateRange const& period, Currency available)
{
   // learn what the growth was for past periods
   if (period.endDate() < QDate::currentDate())
   {
      m_lastDate = period.endDate();
      m_historyTotal += available - m_lastAvailable;
      m_lastAvailable = available;
   }
   // and project the growth into the future
   else
   {
      m_allocation += m_historyTotal.amortize(
                         DateRange(m_firstDate, m_lastDate),
                         DateRange(m_firstDate, m_firstDate)) *
                      period.days();
   }
   return available;
}

void GoalAllocator::budget(QDate const& date)
{
   m_firstDate = date;
   m_historyTotal.clear();
   m_lastAvailable.clear();
   m_lastDate = date;
}

bool GoalAllocator::isUnderfunded() const
{
   return m_allocation.isNegative();
}

void GoalAllocator::spend(Currency const& amount)
{
   m_allocation += amount;
}
