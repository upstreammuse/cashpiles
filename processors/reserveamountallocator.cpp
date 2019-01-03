#include "reserveamountallocator.h"

#include "kernel/daterange.h"

Currency ReserveAmountAllocator::allocate(Currency available)
{
   for (auto it = m_allocations.begin(); it != m_allocations.end(); ++it)
   {
      if (it->total.isNegative())
      {
         available += it->total;
         it->total.clear();
      }
   }
   return available;
}

Currency ReserveAmountAllocator::allocate(DateRange const &period,
                                          Currency available)
{
   for (auto it = m_allocations.begin(); it != m_allocations.end(); ++it)
   {
      DateRange overlap = it->range.intersect(period);

      // in the case where the end of the last period perfectly coincided with
      // the end of the last range of this category
      if (overlap.isNull())
      {
         ++it->range;
         overlap = it->range.intersect(period);
         Q_ASSERT(!overlap.isNull());
      }
      Q_ASSERT(overlap.startDate() == period.startDate());

      Currency amount;

      // in the case where the overlap is the entire period
      if (overlap == period)
      {
         amount += it->amount.amortize(it->range, overlap);
      }
      // in the case where the overlap does not cover the whole period
      else
      {
         amount += it->amount.amortize(it->range, overlap);
         do
         {
            ++it->range;
            overlap = it->range.intersect(period);
            amount += it->amount.amortize(it->range, overlap);
         }
         while (overlap.endDate() != period.endDate());
      }

      it->total += amount;
      available -= amount;
   }
   return available;
}

void ReserveAmountAllocator::budget(QDate const& date, QString const& category,
                                    Currency const &amount,
                                    Interval const& interval)
{
   m_allocations[category].amount = amount;
   m_allocations[category].range = DateRange(date, interval);
}

Currency ReserveAmountAllocator::deallocate(QString const& category)
{
   Currency amount = m_allocations[category].total;
   m_allocations.remove(category);
   return amount;
}

bool ReserveAmountAllocator::isUnderfunded() const
{
   foreach (Allocation const& allocation, m_allocations)
   {
      if (allocation.total.isNegative())
      {
         return true;
      }
   }
   return false;
}

void ReserveAmountAllocator::spend(QString const& category,
                                   Currency const& amount)
{
   m_allocations[category].total += amount;
}