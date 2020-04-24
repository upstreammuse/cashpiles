#pragma once

#include <map>
#include <set>
#include "currency.h"
#include "date.h"
#include "identifier.h"
#include "itemprocessor.h"

class IPTransferBalancer : public ItemProcessor
{
public:
   void finish();
   void processItem(LedgerAccount const& account);
   bool processItem(LedgerBudget const& budget);
   void processItem(LedgerReserve const& reserve);
   void processItem(LedgerTransaction const& transaction);
   void setDateFormat(std::string const& dateFormat);

private:
   void checkTransfers(Date const& date);

private:
   std::set<Identifier> m_accounts;
   std::string m_dateFormat = "yyyy/MM/dd";
   Date m_lastDate;
   std::map<Identifier, std::map<Identifier, Currency>> m_transfers;
};
