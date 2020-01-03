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
   void advanceBudgetPeriod(QString const& filename, uint lineNum,
                            QDate const& date, bool rebudgeting = false);

private:
   struct Goal
   {
      Currency neededThisPeriod;
      Currency reserved;
      Currency reservedThisPeriod;
   };

   struct Reserve
   {
      Currency amount;
      double percentage;
      DateRange period;
      Currency reserved;
   };

private:
   Currency m_available;
   DateRange m_currentPeriod;
   Currency m_currentRoutine;
   Currency m_escrow;
   QHash<QString, Goal> m_goals;
   QSet<QString> m_incomes;
   DateRange m_priorPeriod;
   Currency m_priorRoutine;
   QHash<QString, Reserve> m_reserves;
   QSet<QString> m_routines;
};

#endif
