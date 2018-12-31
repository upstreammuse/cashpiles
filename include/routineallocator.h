#ifndef ROUTINEALLOCATOR_H
#define ROUTINEALLOCATOR_H

#include "currency.h"

class RoutineAllocator
{
public:
   Currency allocate(Currency available);
   Currency allocate(DateRange const& period, QString const& category,
                     Currency available);
   Currency deallocate(QString const& category);
   bool isUnderfunded() const;
   void spend(QDate const& date, QString const& category,
              Currency const& amount);

private:
   QHash<QString, Currency> m_allocations;
   QMap<QDate, QPair<QString, Currency>> m_history;
   QHash<QString, Currency> m_historyTotals;
};

#endif
