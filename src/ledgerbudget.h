#pragma once

#include <memory>
#include <vector>
#include "currency.h"
#include "date.h"
#include "interval.h"
#include "ledgerbudgetentry.h"
#include "ledgeritem.h"

class LedgerBudget : public LedgerItem
{
public:
   LedgerBudget(Date const& date, std::string const& fileName, size_t lineNum);

   Date date() const;
   void setDate(Date const& date);

   Interval interval() const;
   void setInterval(Interval interval);

   void appendEntry(std::shared_ptr<LedgerBudgetEntry> entry);

   void processItem(ItemProcessor& processor) const;

private:
   Date m_date;
   std::vector<std::shared_ptr<LedgerBudgetEntry>> m_entries;
   Interval m_interval;
};
