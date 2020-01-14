#ifndef LEDGERBUDGET_H
#define LEDGERBUDGET_H

#include <QDate>
#include <QSet>
#include "currency.h"
#include "interval.h"
#include "ledgerbudgetentry.h"
#include "ledgeritem.h"

class LedgerBudget : public LedgerItem
{
public:
   LedgerBudget(QString const& fileName, uint lineNum);

   QDate date() const;
   void setDate(QDate const& date);

   Interval interval() const;
   void setInterval(Interval interval);

   void appendEntry(QSharedPointer<LedgerBudgetEntry> entry);

   void processItem(ItemProcessor& processor) const;

private:
   QDate m_date;
   QList<QSharedPointer<LedgerBudgetEntry>> m_entries;
   Interval m_interval;
};

#endif
