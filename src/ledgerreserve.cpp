#include "ledgerreserve.h"

#include "itemprocessor.h"

LedgerReserveEntry::LedgerReserveEntry(QString const& filename, uint linenum) :
   LedgerItem(filename, linenum)
{
}

Currency LedgerReserveEntry::amount() const
{
   return m_amount;
}

QString LedgerReserveEntry::category() const
{
   return m_category;
}

QDate LedgerReserveEntry::date() const
{
   return m_date;
}

bool LedgerReserveEntry::isOwner() const
{
   return m_owner;
}

void LedgerReserveEntry::processItem(ItemProcessor& processor) const
{
   processor.processItem(*this);
}

void LedgerReserveEntry::setAmount(Currency const& amount)
{
   m_amount = amount;
}

void LedgerReserveEntry::setCategory(QString const& category)
{
   m_category = category;
}

void LedgerReserveEntry::setDate(QDate const& date)
{
   m_date = date;
}

void LedgerReserveEntry::setOwner(bool owner)
{
   m_owner = owner;
}

LedgerReserve::LedgerReserve(QString const& filename, uint linenum) :
   LedgerItem(filename, linenum)
{
}

Currency LedgerReserve::amount() const
{
   Currency amount;
   foreach (auto entry, m_entries)
   {
      amount += entry->amount();
   }
   return amount;
}

void LedgerReserve::appendEntry(QSharedPointer<LedgerReserveEntry> entry)
{
   entry->setDate(m_date);
   m_entries.append(entry);
}

QDate LedgerReserve::date() const
{
   return m_date;
}

int LedgerReserve::numEntries() const
{
   return m_entries.size();
}

void LedgerReserve::processItem(ItemProcessor& processor) const
{
   processor.processItem(*this);
   foreach (auto entry, m_entries)
   {
      entry->processItem(processor);
   }
}

void LedgerReserve::setDate(QDate const& date)
{
   m_date = date;
   foreach (auto entry, m_entries)
   {
      entry->setDate(date);
   }
}
