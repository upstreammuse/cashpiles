#ifndef ACCOUNTBALANCER_H
#define ACCOUNTBALANCER_H

#include <QHash>
#include <currency.h>
#include "itemprocessor.h"

class AccountBalancer : public ItemProcessor
{
public:
   AccountBalancer(QObject* parent);
   void processItem(LedgerAccountCommand const& account);
   void processItem(LedgerBudget const& budget);
   void processItem(LedgerComment const& comment);
   void processItem(LedgerTransaction const& transaction);
   void stop();

private:
   QHash<QString, Currency> m_accounts;
   QHash<QString, QHash<QString, Currency>> m_transfers;
};

#endif
