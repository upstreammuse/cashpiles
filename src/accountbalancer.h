#ifndef ACCOUNTBALANCER_H
#define ACCOUNTBALANCER_H

#include <QDate>
#include "currency.h"
#include "identifier.h"
#include "itemprocessor.h"

class AccountBalancer : public ItemProcessor
{
public:
   AccountBalancer(QDate const& today);
   void finish();
   void processItem(LedgerAccount const& account);
   void processItem(LedgerTransaction const& transaction);

private:
   struct Account
   {
      Currency balance;
      Currency cleared;
      Currency future;
      bool onBudget;
   };

private:
   QMap<QString, Account> m_accounts;
   bool m_hasPending = false;
   QDate const m_today;
};

#endif
