#include "ledgerreserve.h"

#include <cassert>
#include "itemprocessor.h"

LedgerReserveEntry::LedgerReserveEntry(std::string const& filename,
                                       size_t linenum) :
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
   assert(category.type() == Identifier::Type::CATEGORY ||
          category.type() == Identifier::Type::OWNER);
   m_category = category;
}

Date LedgerReserveEntry::date() const
{
   return m_date;
}

void LedgerReserveEntry::setDate(Date const& date)
{
   assert(date.isValid());
   m_date = date;
}

void LedgerReserveEntry::processItem(ItemProcessor& processor) const
{
   processor.processItem(*this);
}

LedgerReserve::LedgerReserve(std::string const& filename, size_t linenum) :
   LedgerItem(filename, linenum)
{
}

Currency LedgerReserve::amount() const
{
   Currency amount;
   for (auto entry : m_entries)
   {
      amount += entry->amount();
   }
   return amount;
}

Date LedgerReserve::date() const
{
   return m_date;
}

void LedgerReserve::setDate(Date const& date)
{
   assert(date.isValid());
   m_date = date;
   for (auto entry : m_entries)
   {
      entry->setDate(date);
   }
}

void LedgerReserve::appendEntry(std::shared_ptr<LedgerReserveEntry> entry)
{
   entry->setDate(m_date);
   m_entries.push_back(entry);
}

size_t LedgerReserve::numEntries() const
{
   return m_entries.size();
}

void LedgerReserve::processItem(ItemProcessor& processor) const
{
   if (processor.processItem(*this))
   {
      for (auto entry : m_entries)
      {
         entry->processItem(processor);
      }
   }
}
