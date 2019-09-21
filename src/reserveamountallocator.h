#ifndef RESERVEAMOUNTALLOCATOR_H
#define RESERVEAMOUNTALLOCATOR_H

#include "currency.h"
#include "daterange.h"
#include "interval.h"

class ReserveAmountAllocator
{
public:
   Currency allocate(Currency available);
   Currency allocate(DateRange const& period, Currency available);
   void budget(QDate const& date, QString const& category,
               Currency const& amount, Interval const& interval);
   Currency deallocate(QString const& category);
   bool isUnderfunded() const;
   void spend(QString const& category, Currency const& amount);

private:
   struct Allocation
   {
      Currency amount;
      DateRange range;
      Currency total;
   };

private:
   QHash<QString, Allocation> m_allocations;
};

#endif
