#include "ledgerbudget.h"

#include <cassert>
#include "itemprocessor.h"

LedgerBudget::LedgerBudget(
      Date const& date, Interval const& interval, std::string const& fileName,
      size_t lineNum) :
   LedgerItem(fileName, lineNum),
   m_date(date),
   m_interval(interval)
{
}

LedgerBudget::LedgerBudget(
      Date const& date, Interval const& interval, LedgerItem const& item) :
   LedgerItem(item),
   m_date(date),
   m_interval(interval)
{
}

Date LedgerBudget::date() const
{
   return m_date;
}

void LedgerBudget::setDate(Date const& date)
{
   m_date = date;
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
   m_entries.push_back(entry);
}

void LedgerBudget::processItem(ItemProcessor& processor) const
{
   if (processor.processItem(*this))
   {
      for (auto entry : m_entries)
      {
         entry->processItem(processor);
      }
      processor.finishBudget();
   }
}
