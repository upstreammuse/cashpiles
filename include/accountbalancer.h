#ifndef ACCOUNTBALANCER_H
#define ACCOUNTBALANCER_H

#include <QHash>
#include "itemprocessor.h"

class AccountBalancer : public ItemProcessor
{
public:
   AccountBalancer(QObject* parent);
   void processItem(LedgerAccountCommand const& account);
   void processItem(LedgerBudgetAllocation const& allocation);
   void processItem(LedgerComment const& comment);
   void processItem(LedgerTransaction const& transaction);

private:
   QHash<QString, int> m_accounts;
};

#endif
