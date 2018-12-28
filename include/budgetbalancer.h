#ifndef BUDGETBALANCER_H
#define BUDGETBALANCER_H

#include <QDate>
#include "currency.h"
#include "interval.h"
#include "itemprocessor.h"

class BudgetBalancer : public ItemProcessor
{
public:
   BudgetBalancer(QObject* parent);
   void processItem(LedgerAccountCommand const& account);
   void processItem(LedgerAllocation const& allocation);
   void processItem(LedgerBudget const& budget);
   void processItem(LedgerComment const& comment);
   void processItem(LedgerTransaction const& transaction);
   void stop();

private:
   QHash<QString, bool> m_accounts;
   QDate m_budgetDate;
   Interval m_budgetInterval;
   QHash<QString, Currency> m_categories;
   QHash<QString, Currency> m_totals;
};

#endif
