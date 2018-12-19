#ifndef BUDGETBALANCER_H
#define BUDGETBALANCER_H

#include "currency.h"
#include "itemprocessor.h"

class BudgetBalancer : public ItemProcessor
{
public:
   BudgetBalancer(QObject* parent);
   void processItem(LedgerAccountCommand const& account);
   void processItem(LedgerBudgetAllocation const& allocation);
   void processItem(LedgerComment const& comment);
   void processItem(LedgerTransaction const& transaction);
   void stop();

private:
   QHash<QString, bool> m_accounts;
   QHash<QString, Currency> m_categories;
};

#endif
