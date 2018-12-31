#ifndef ROUTINEALLOCATOR_H
#define ROUTINEALLOCATOR_H

#include "currency.h"

class RoutineAllocator
{
public:
   Currency allocate(QString const& category, Currency available);
   Currency allocate(DateRange const& period, QString const& category,
                     Currency available);
   Currency amountAllocated() const;
   Currency deallocate(QString const& category);
   void spend(QDate const& date, QString const& category,
              Currency const& amount);

private:
   QHash<QString, Currency> m_allocations;
   QMap<QDate, QPair<QString, Currency>> m_history;
   QHash<QString, Currency> m_historyTotals;
};

#endif
