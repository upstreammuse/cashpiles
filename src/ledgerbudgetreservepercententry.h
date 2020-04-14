#pragma once

#include "ledgerbudgetentry.h"

class LedgerBudgetReservePercentEntry : public LedgerBudgetEntry
{
public:
   LedgerBudgetReservePercentEntry(std::string const& filename, size_t lineNum);

   unsigned int percentage() const;
   void setPercentage(unsigned int percentage);

   void processItem(ItemProcessor& processor) const;

private:
   unsigned int m_percentage;
};
