#include "ledgerbudgetreservepercententry.h"

#include "itemprocessor.h"

LedgerBudgetReservePercentEntry::LedgerBudgetReservePercentEntry(
      QString const& filename, uint lineNum) :
   LedgerBudgetEntry(filename, lineNum)
{
}

uint LedgerBudgetReservePercentEntry::percentage() const
{
   return m_percentage;
}

void LedgerBudgetReservePercentEntry::setPercentage(uint percentage)
{
   m_percentage = percentage;
}

void LedgerBudgetReservePercentEntry::processItem(
      ItemProcessor& processor) const
{
   processor.processItem(*this);
}
