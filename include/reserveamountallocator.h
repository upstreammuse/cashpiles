#ifndef RESERVEAMOUNTALLOCATOR_H
#define RESERVEAMOUNTALLOCATOR_H

#include "currency.h"
#include "interval.h"

class ReserveAmountAllocator
{
public:
   Currency allocate(Currency available);
   Currency allocate(DateRange const& period, QString const& category,
                     Currency available);
   void budget(QDate const& date, QString const& category,
               Currency const& amount, Interval const& interval);
   Currency deallocate(QString const& category);
   bool isUnderfunded() const;
   void spend(QString const& category, Currency const& amount);

private:
   QHash<QString, Currency> m_allocations;
   QHash<QString, Currency> m_amounts;
   QHash<QString, DateRange> m_ranges;
};

#endif
