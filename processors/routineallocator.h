#ifndef ROUTINEALLOCATOR_H
#define ROUTINEALLOCATOR_H

#include "model/currency.h"

class RoutineAllocator
{
public:
   Currency allocate(Currency available);
   Currency allocate(DateRange const& period, Currency available);
   void deallocate(QString const& category);
   bool isUnderfunded() const;
   void spend(QDate const& date, QString const& category,
              Currency const& amount);

private:
   Currency m_allocation;
   QMap<QDate, QPair<QString, Currency>> m_history;
   QHash<QString, Currency> m_historyTotals;
};

#endif
