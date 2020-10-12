#pragma once

#include <map>
#include "currency.h"
#include "date.h"
#include "itemprocessor.h"
#include "ledgertransactionv2.h"
#include "reportaccount.h"

class Report;

class IPAccountBalancer : public ItemProcessor
{
public:
   explicit IPAccountBalancer(Report& report);
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
   void ensureOpen(Date const& date, std::string const& account, bool onBudget);
   std::shared_ptr<ReportAccount> report(
         Date const& date, std::string const& account);

private:
   struct Account
   {
      Currency balance;
      bool hasPending = false;
      bool isClosed = true;
      bool onBudget = true;
   };

private:
   std::map<std::string, Account> m_accounts;
   Report& m_report;
   std::map<std::string, std::shared_ptr<ReportAccount>> m_reportItems;
   Date m_workingDate;
   std::string m_workingPayee;
   LedgerTransactionV2::Status m_workingStatus;
};
