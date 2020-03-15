#ifndef IPACCOUNTBALANCER_H
#define IPACCOUNTBALANCER_H

#include <QDate>
#include "currency.h"
#include "date.h"
#include "identifier.h"
#include "itemprocessor.h"

class IPAccountBalancer : public ItemProcessor
{
public:
   IPAccountBalancer(Date const& today);
   void finish();
   void processItem(LedgerAccount const& account);
   void processItem(LedgerTransaction const& transaction);

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
   QMap<Identifier, Account> m_accounts;
   QDate const m_today;
};

#endif
