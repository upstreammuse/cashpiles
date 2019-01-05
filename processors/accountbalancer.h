#ifndef ACCOUNTBALANCER_H
#define ACCOUNTBALANCER_H

#include <QDate>
#include <QHash>
#include "kernel/currency.h"
#include "kernel/itemprocessor.h"
#include "kernel/ledgeritem.h"

class AccountBalancerUI;

class AccountBalancer : public ItemProcessor
{
   Q_OBJECT

public:
   AccountBalancer(QObject* parent);
   void processItem(LedgerAccountBalance const& balance);
   void processItem(LedgerAccountCommand const& account);
   void processItem(LedgerBudget const& budget);
   void processItem(LedgerComment const& comment);
   void processItem(LedgerTransaction const& transaction);
   void stop();

signals:
   void balance(QString const&, bool onbudget, Currency const&) const;
   void message(QString const&) const;
   void message(LedgerItem const&, QString const&) const;

private:
   void checkTransfers(QDate const& date);

private:
   struct Account
   {
      bool onbudget;
      Currency balance;
   };

private:
   QHash<QString, Account> m_accounts;
   QDate m_lastDate;
   QHash<QString, QHash<QString, Currency>> m_transfers;
};

#endif
