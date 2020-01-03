#ifndef ACCOUNTBALANCER_H
#define ACCOUNTBALANCER_H

#include <QDate>
#include "currency.h"
#include "itemprocessor.h"

class AccountBalancer : public ItemProcessor
{
public:
   void finish();
   void processItem(LedgerAccount const& account);
   void processItem(LedgerBudget const& budget);
   void processItem(LedgerTransaction const& transaction);

private:
   void checkTransfers(QDate const& date);

private:
   struct Account
   {
      bool onBudget;
      Currency balance;
   };

private:
   QHash<QString, Account> m_accounts;
   QDate m_lastDate;
   QHash<QString, QHash<QString, Currency>> m_transfers;
};

#endif
