#include "ledgerbudgetreserveamountentry.h"

#include "itemprocessor.h"

LedgerBudgetReserveAmountEntry::LedgerBudgetReserveAmountEntry(
      QString const& filename, uint lineNum) :
   LedgerBudgetEntry(filename, lineNum)
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
