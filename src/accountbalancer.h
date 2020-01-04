#ifndef ACCOUNTBALANCER_H
#define ACCOUNTBALANCER_H

#include <QDate>
#include "currency.h"
#include "itemprocessor.h"

// TODO this processes transactions into the future, which is OK, but it should
// have a "future" category in the output table to make that obvious

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
      Currency balance;
      Currency cleared;
      bool onBudget;
   };

private:
   QMap<QString, Account> m_accounts;
   bool m_hasPending = false;
   QDate m_lastDate;
   QHash<QString, QHash<QString, Currency>> m_transfers;
};

#endif
