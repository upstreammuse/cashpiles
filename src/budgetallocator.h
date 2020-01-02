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
   void processItem(LedgerAccount const& account);
   void processItem(LedgerBudget const& budget);
   void processItem(LedgerBudgetGoalEntry const& budget);
   void processItem(LedgerBudgetIncomeEntry const& budget);
   void processItem(LedgerBudgetReserveAmountEntry const& budget);
   void processItem(LedgerBudgetReservePercentEntry const& budget);
   void processItem(LedgerBudgetRoutineEntry const& budget);
   void processItem(LedgerReserve const& reserve);
   void processItem(LedgerTransaction const& transaction);

private:
   void advanceBudgetPeriod(QDate const& date, bool rebudgeting = false);

private:
   Currency m_available;
   DateRange m_currentPeriod;
   Currency m_currentRoutine;
   Currency m_escrow;
   QHash<QString, Currency> m_goals;
   QHash<QString, Currency> m_goalThisPeriod;
   QSet<QString> m_incomes;
   QHash<QString, Currency> m_needToReserve;
   DateRange m_priorPeriod;
   Currency m_priorRoutine;
   QHash<QString, Currency> m_reserveAmounts;
   QHash<QString, double> m_reservePercentages;
   QHash<QString, DateRange> m_reservePeriods;
   QHash<QString, Currency> m_reserves;
   QSet<QString> m_routines;
};

#endif
