#pragma once

#include "ledgerbudgetentry.h"

class LedgerBudgetReservePercentEntry : public LedgerBudgetEntry
{
public:
   LedgerBudgetReservePercentEntry(QString const& filename, uint lineNum);

   uint percentage() const;
   void setPercentage(uint percentage);

   void processItem(ItemProcessor& processor) const;

private:
   uint m_percentage;
};
