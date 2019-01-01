#ifndef RESERVEPERCENTALLOCATOR_H
#define RESERVEPERCENTALLOCATOR_H

#include "model/currency.h"

class ReservePercentAllocator
{
public:
   Currency allocate(Currency available);
   Currency allocate(Currency const& income, Currency available);
   void budget(QString const& category, int percentage);
   Currency deallocate(QString const& category);
   bool isUnderfunded() const;
   void spend(QString const& category, Currency const& amount);

private:
   QHash<QString, Currency> m_allocations;
   QHash<QString, int> m_percentages;
};

#endif
