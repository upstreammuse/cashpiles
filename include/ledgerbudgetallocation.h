#ifndef LEDGERBUDGETALLOCATION_H
#define LEDGERBUDGETALLOCATION_H

#include <QDate>
#include <QMap>
#include "ledgercommand.h"

class LedgerBudgetAllocation : public LedgerCommand
{
public:
   LedgerBudgetAllocation(int item, int line);
   QMap<QString, int> allocations() const;
   void appendAllocation(QString const& category, int amount);
   QDate date() const;
   void setDate(QDate const& date);

private:
   QMap<QString, int> m_allocations;
   QDate m_date;
};

#endif
