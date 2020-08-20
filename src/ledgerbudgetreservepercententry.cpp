#include "ledgerbudgetreservepercententry.h"

#include "itemprocessor.h"

unsigned int LedgerBudgetReservePercentEntry::percentage() const
{
   return m_percentage;
}

void LedgerBudgetReservePercentEntry::setPercentage(unsigned int percentage)
{
   m_percentage = percentage;
}

void LedgerBudgetReservePercentEntry::processItem(
      ItemProcessor& processor) const
{
   processor.processItem(*this);
}
