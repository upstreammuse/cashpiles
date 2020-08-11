#pragma once

#include "date.h"
#include "itemprocessor.h"

class IPDateValidator : public ItemProcessor
{
public:
   IPDateValidator(std::string const& dateFormat);
   void processItem(LedgerAccount const& account);
   void processItem(LedgerAccountBalance const& balance);
   bool processItem(LedgerBudget const& budget);
   void processItem(LedgerTransaction const& transaction);
   bool processItem(LedgerTransactionV2 const& transaction);

private:
   void processDate(Date const& date, std::string const& fileName,
                    size_t lineNum);

private:
   std::string m_dateFormat;
   Date m_latestDate;
};
