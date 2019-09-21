#ifndef INCOMEALLOCATOR_H
#define INCOMEALLOCATOR_H

#include <QSet>
#include "currency.h"
#include "itemprocessor.h"

class IncomeAllocator : public ItemProcessor
{
public:
   void finish();
   void processItem(LedgerBudgetIncomeEntry const& budget);
   void processItem(LedgerBudgetReservePercentEntry const& budget);
   void processItem(LedgerTransaction const& transaction);

private:
   Currency m_income;
   QSet<QString> m_incomeCategories;
   QHash<QString, double> m_reserveCategories;
   Currency m_reserved;
   Currency m_spent;
};

#endif
