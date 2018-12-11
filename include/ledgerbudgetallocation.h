#ifndef LEDGERBUDGETALLOCATION_H
#define LEDGERBUDGETALLOCATION_H

#include <QDate>
#include <QMap>
#include "ledgeritem.h"

class LedgerBudgetAllocation : public LedgerItem
{
public:
   LedgerBudgetAllocation(QString const& filename, int lineNum);

   QMap<QString, int> allocations() const;
   void appendAllocation(QString const& category, int amount);

   QDate date() const;
   void setDate(QDate const& date);

   void processItem(ItemProcessor* processor);

private:
   QMap<QString, int> m_allocations;
   QDate m_date;
};

#endif
