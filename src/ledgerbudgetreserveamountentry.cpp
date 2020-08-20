#include "ledgerbudgetreserveamountentry.h"

#include "itemprocessor.h"

LedgerBudgetReserveAmountEntry::LedgerBudgetReserveAmountEntry(
      Interval const& interval, std::string const& filename, size_t lineNum) :
   LedgerBudgetEntry(filename, lineNum),
   m_interval(interval)
{
}

LedgerBudgetReserveAmountEntry::LedgerBudgetReserveAmountEntry(
      Interval const& interval, LedgerItem const& item) :
   LedgerBudgetEntry(item),
   m_interval(interval)
{
}

Currency LedgerBudgetReserveAmountEntry::amount() const
{
   return m_amount;
}

void LedgerBudgetReserveAmountEntry::setAmount(Currency const& amount)
{
   m_amount = amount;
}

Interval LedgerBudgetReserveAmountEntry::interval() const
{
   return m_interval;
}

void LedgerBudgetReserveAmountEntry::setInterval(Interval const& interval)
{
   m_interval = interval;
}

void LedgerBudgetReserveAmountEntry::processItem(ItemProcessor& processor) const
{
   processor.processItem(*this);
}
