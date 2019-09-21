#ifndef GOALALLOCATOR_H
#define GOALALLOCATOR_H

#include <QDate>
#include "currency.h"

class GoalAllocator
{
public:
   Currency allocate(Currency available);
   Currency allocate(DateRange const& period, Currency available);
   void budget(QDate const& date);
   bool isUnderfunded() const;
   void spend(Currency const& amount);

private:
   Currency m_allocation;
   QDate m_firstDate;
   Currency m_historyTotal;
   Currency m_lastAvailable;
   QDate m_lastDate;
};

#endif
