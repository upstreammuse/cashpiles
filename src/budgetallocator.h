#ifndef BUDGETALLOCATOR_H
#define BUDGETALLOCATOR_H

#include <QHash>
#include <QSet>
#include "currency.h"
#include "daterange.h"
#include "itemprocessor.h"

class BudgetAllocator : public ItemProcessor
{
public:
   void finish();
   void processItem(LedgerBudget const& budget);
   void processItem(LedgerBudgetIncomeEntry const& budget);
   void processItem(LedgerBudgetReservePercentEntry const& budget);
   void processItem(LedgerBudgetRoutineEntry const& budget);
   void processItem(LedgerTransaction const& transaction);

private:
   void refreshCurrentPeriod(QDate const& date, bool ignoreIfFirstDay = false);

private:
   Currency m_available;
   DateRange m_currentPeriod;
   Currency m_currentRoutine;
   Currency m_escrow;
   QSet<QString> m_incomes;
   DateRange m_priorPeriod;
   Currency m_priorRoutine;
   QHash<QString, double> m_reservePercentages;
   QHash<QString, Currency> m_reserves;
   QSet<QString> m_routines;
};

#endif
