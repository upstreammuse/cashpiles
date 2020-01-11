#ifndef ACCOUNTBALANCER_H
#define ACCOUNTBALANCER_H

#include <QDate>
#include "currency.h"
#include "itemprocessor.h"

class AccountBalancer : public ItemProcessor
{
public:
   AccountBalancer(QDate const& today);
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
      Currency future;
      bool onBudget;
   };

private:
   QMap<QString, Account> m_accounts;
   bool m_hasPending = false;
   QDate m_lastDate;
   QDate m_today;
   QHash<QString, QHash<QString, Currency>> m_transfers;
};

#endif
