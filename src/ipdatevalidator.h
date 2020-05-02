#pragma once

#include "date.h"
#include "itemprocessor.h"

class IPDateValidator : public ItemProcessor
{
public:
   void processItem(LedgerAccount const& account);
   void processItem(LedgerBudget const& budget);
   void processItem(LedgerTransaction const& transaction);

private:
   void processDate(Date const& date, std::string const& fileName,
                    size_t lineNum);

private:
   Date m_latestDate;
};
