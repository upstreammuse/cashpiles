#pragma once

#include "ledgerbudgetentry.h"

class LedgerBudgetReservePercentEntry : public LedgerBudgetEntry
{
public:
   using LedgerBudgetEntry::LedgerBudgetEntry;

   unsigned int percentage() const;
   void setPercentage(unsigned int percentage);

   void processItem(ItemProcessor& processor) const;

private:
   unsigned int m_percentage;
};
