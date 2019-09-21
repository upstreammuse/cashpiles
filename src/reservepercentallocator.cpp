#include "reservepercentallocator.h"

Currency ReservePercentAllocator::allocate(Currency available)
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

Currency ReservePercentAllocator::allocate(Currency const& income,
                                           Currency available)
{
   for (auto it = m_allocations.begin(); it != m_allocations.end(); ++it)
   {
      Currency amount = income * (m_percentages[it.key()] / 100.0);
      *it += amount;
      available -= amount;
   }
   return available;
}

void ReservePercentAllocator::budget(QString const& category, int percentage)
{
   m_percentages[category] = percentage;
}

Currency ReservePercentAllocator::deallocate(QString const& category)
{
   Currency amount = m_allocations[category];
   m_allocations.remove(category);
   m_percentages.remove(category);
   return amount;
}

bool ReservePercentAllocator::isUnderfunded() const
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

void ReservePercentAllocator::spend(QString const& category,
                                    Currency const& amount)
{
   m_allocations[category] += amount;
}
