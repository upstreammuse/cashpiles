#pragma once

#include "ledgerbudgetentry.h"

class LedgerBudgetReserveAmountEntry : public LedgerBudgetEntry
{
public:
   LedgerBudgetReserveAmountEntry(
         Interval const& interval, std::string const& filename, size_t lineNum);
   LedgerBudgetReserveAmountEntry(Interval const& interval, LedgerItem const&);

   Currency amount() const;
   void setAmount(Currency const& amount);

   Interval interval() const;
   void setInterval(Interval const& interval);

   void processItem(ItemProcessor& processor) const;

private:
   Currency m_amount;
   Interval m_interval;
};
