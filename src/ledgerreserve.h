#ifndef LEDGERRESERVE_H
#define LEDGERRESERVE_H

#include <QDate>
#include <QSharedPointer>
#include "currency.h"
#include "ledgeritem.h"

class LedgerReserveEntry : public LedgerItem
{
public:
   LedgerReserveEntry(QString const& filename, uint linenum);
   Currency amount() const;
   QString category() const;
   QDate date() const;
   bool isOwner() const;
   void processItem(ItemProcessor& processor) const;
   void setAmount(Currency const& amount);
   void setCategory(QString const& category);
   void setDate(QDate const& date);
   void setOwner(bool owner);

private:
   Currency m_amount;
   QString m_category;
   QDate m_date;
   bool m_owner = false;
};

class LedgerReserve : public LedgerItem
{
public:
   LedgerReserve(QString const& filename, uint linenum);
   Currency amount() const;
   void appendEntry(QSharedPointer<LedgerReserveEntry> entry);
   QDate date() const;
   int numEntries() const;
   void processItem(ItemProcessor& processor) const;
   void setDate(QDate const& date);

private:
   QDate m_date;
   QList<QSharedPointer<LedgerReserveEntry>> m_entries;
};

#endif
