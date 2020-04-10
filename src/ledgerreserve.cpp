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

void LedgerReserveEntry::setAmount(Currency const& amount)
{
   m_amount = amount;
}

Identifier LedgerReserveEntry::category() const
{
   return m_category;
}

void LedgerReserveEntry::setCategory(Identifier const& category)
{
   Q_ASSERT(category.type() == Identifier::Type::CATEGORY ||
            category.type() == Identifier::Type::OWNER);
   m_category = category;
}

QDate LedgerReserveEntry::date() const
{
   return m_date;
}

void LedgerReserveEntry::setDate(QDate const& date)
{
   Q_ASSERT(date.isValid());
   m_date = date;
}

void LedgerReserveEntry::processItem(ItemProcessor& processor) const
{
   processor.processItem(*this);
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

QDate LedgerReserve::date() const
{
   return m_date;
}

void LedgerReserve::setDate(QDate const& date)
{
   Q_ASSERT(date.isValid());
   m_date = date;
   foreach (auto entry, m_entries)
   {
      entry->setDate(date);
   }
}

void LedgerReserve::appendEntry(std::shared_ptr<LedgerReserveEntry> entry)
{
   entry->setDate(m_date);
   m_entries.push_back(entry);
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
