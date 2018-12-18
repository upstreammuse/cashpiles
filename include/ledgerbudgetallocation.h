#ifndef LEDGERBUDGETALLOCATION_H
#define LEDGERBUDGETALLOCATION_H

#include <QDate>
#include <QMap>
#include "currency.h"
#include "ledgeritem.h"

class LedgerBudgetAllocation : public LedgerItem
{
public:
   LedgerBudgetAllocation(QString const& filename, int lineNum);

   QMap<QString, Currency> allocations() const;
   void appendAllocation(QString const& category, Currency const& amount);

   QDate date() const;
   void setDate(QDate const& date);

   void processItem(ItemProcessor* processor);

private:
   QMap<QString, Currency> m_allocations;
   QDate m_date;
};

#endif
