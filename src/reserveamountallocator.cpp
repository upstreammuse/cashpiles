#include "reserveamountallocator.h"

#include <iostream>
#include "daterange.h"

Currency ReserveAmountAllocator::allocate(Currency available)
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

Currency ReserveAmountAllocator::allocate(DateRange const &period,
                                          QString const& category,
                                          Currency available)
{
   DateRange& range = m_ranges[category];
   DateRange overlap = range.intersect(period);

   // in the case where the end of the last period perfectly coincided with the
   // end of the last range of this category
   if (overlap.isNull())
   {
      ++range;
      overlap = range.intersect(period);
      Q_ASSERT(!overlap.isNull());
   }

   Currency amount;

   // in the case where the overlap is the entire period
   if (overlap == period)
   {
      amount += m_amounts[category].amortize(range, overlap);
   }
   // in the case where the overlap is at the beginning of the period
   // but does not cover the whole period
   else if (overlap.startDate() == period.startDate())
   {
      amount += m_amounts[category].amortize(range, overlap);
      do
      {
         ++range;
         overlap = range.intersect(period);
         amount += m_amounts[category].amortize(range, overlap);
      }
      while (overlap.endDate() != period.endDate());
   }
   // otherwise we screwed up and advanced the reserve range too far
   // somewhere else
   else
   {
      std::cerr << "Internal error: reserve period advanced too far"
                << std::endl;
   }

   m_allocations[category] += amount;
   available -= amount;
   return available;
}

void ReserveAmountAllocator::budget(QDate const& date, QString const& category,
                                    Currency const &amount,
                                    Interval const& interval)
{
   m_amounts[category] = amount;
   m_ranges[category] = DateRange(date, interval);
}

Currency ReserveAmountAllocator::deallocate(QString const& category)
{
   Currency amount = m_allocations[category];
   m_allocations.remove(category);
   return amount;
}

bool ReserveAmountAllocator::isUnderfunded() const
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

void ReserveAmountAllocator::spend(QString const& category,
                                   Currency const& amount)
{
   m_allocations[category] += amount;
}
