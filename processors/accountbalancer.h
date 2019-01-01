#ifndef ACCOUNTBALANCER_H
#define ACCOUNTBALANCER_H

#include <QDate>
#include <QHash>
#include "model/currency.h"
#include "model/itemprocessor.h"
#include "model/ledgeritem.h"

class AccountBalancerUI;

class AccountBalancer : public ItemProcessor
{
   Q_OBJECT

public:
   AccountBalancer(AccountBalancerUI* ui, QObject* parent);
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
