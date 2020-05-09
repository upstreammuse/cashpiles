#pragma once

#include <map>
#include "currency.h"
#include "date.h"
#include "itemprocessor.h"
#include "ledgertransactionv2.h"

class IPAccountBalancer : public ItemProcessor
{
public:
   IPAccountBalancer(Date const& today);
   Currency budgetable() const;
   void finish();
   void processItem(LedgerAccount const& account);
   void processItem(LedgerAccountBalance const& balance);
   void processItem(LedgerTransaction const& transaction);
   bool processItem(LedgerTransactionV2 const& transaction);
   void processItem(LedgerTransactionV2AccountEntry const& entry);
   void processItem(LedgerTransactionV2CategoryEntry const& entry);
   void processItem(LedgerTransactionV2OwnerEntry const& entry);

private:
   struct Account
   {
      Currency balance;
      Currency cleared;
      Currency future;
      bool hasPending = false;
      bool isClosed = true;
      bool onBudget = true;
   };

private:
   std::map<std::string, Account> m_accounts;
   Date const m_today;
   Date m_workingDate;
   LedgerTransactionV2::Status m_workingStatus;
};
