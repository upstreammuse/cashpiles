#ifndef BUDGETBALANCER_H
#define BUDGETBALANCER_H

#include <QDate>
#include "ledgerbudget.h"
#include "currency.h"
#include "interval.h"
#include "itemprocessor.h"

class BudgetBalancer : public ItemProcessor
{
public:
   BudgetBalancer(QObject* parent);
   void processItem(LedgerAccountCommand const& account);
   void processItem(LedgerBudget const& budget);
   void processItem(LedgerComment const& comment);
   void processItem(LedgerTransaction const& transaction);
   void stop();

private:
   void advancePeriodToDate(QDate const& date);
   void allocateCategories();
   int priorDays();

private:
   QHash<QString, bool> m_accounts;
   Currency m_available;
   QHash<QString, BudgetCategory> m_categories;
   QDate m_firstDate;
   QDate m_periodEnd;
   Interval m_periodLength;
   QDate m_periodStart;
   QHash<QString, Currency> m_reserves;
   Currency m_routineEscrow;
   Currency m_routineTotal;
};

#endif
