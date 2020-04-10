#ifndef LEDGERRESERVE_H
#define LEDGERRESERVE_H

#include <memory>
#include <QDate>
#include <QSharedPointer>
#include "currency.h"
#include "identifier.h"
#include "ledgeritem.h"

class LedgerReserveEntry : public LedgerItem
{
public:
   LedgerReserveEntry(QString const& filename, uint linenum);

   Currency amount() const;
   void setAmount(Currency const& amount);

   Identifier category() const;
   void setCategory(Identifier const& category);

   QDate date() const;
   void setDate(QDate const& date);

   void processItem(ItemProcessor& processor) const;

private:
   Currency m_amount;
   Identifier m_category;
   QDate m_date;
};

class LedgerReserve : public LedgerItem
{
public:
   LedgerReserve(QString const& filename, uint linenum);

   Currency amount() const;

   QDate date() const;
   void setDate(QDate const& date);

   void appendEntry(std::shared_ptr<LedgerReserveEntry> entry);
   int numEntries() const;

   void processItem(ItemProcessor& processor) const;

private:
   QDate m_date;
   std::vector<std::shared_ptr<LedgerReserveEntry>> m_entries;
};

#endif
