#include "ledgerbudget.h"

#include <cassert>
#include "itemprocessor.h"

LedgerBudget::LedgerBudget(std::string const& fileName, size_t lineNum) :
   LedgerItem(fileName, lineNum)
{
}

Date LedgerBudget::date() const
{
   return m_date;
}

void LedgerBudget::setDate(Date const& date)
{
   assert(date.isValid());
   m_date = date;
   for (auto entry : m_entries)
   {
      entry->setDate(m_date);
   }
}

Interval LedgerBudget::interval() const
{
   return m_interval;
}

void LedgerBudget::setInterval(Interval interval)
{
   m_interval = interval;
}

void LedgerBudget::appendEntry(std::shared_ptr<LedgerBudgetEntry> entry)
{
   entry->setDate(m_date);
   m_entries.push_back(entry);
}

void LedgerBudget::processItem(ItemProcessor& processor) const
{
   processor.processItem(*this);
   for (auto entry : m_entries)
   {
      entry->processItem(processor);
   }
}
