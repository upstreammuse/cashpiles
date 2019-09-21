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
   void appendEntry(QSharedPointer<LedgerBudgetEntry> entry);
   QDate date() const;
   Interval interval() const;
   void processItem(ItemProcessor& processor) const;
   void setDate(QDate const& date);
   void setInterval(Interval interval);

private:
   QDate m_date;
   QList<QSharedPointer<LedgerBudgetEntry>> m_entries;
   Interval m_interval;
};

#endif
