#ifndef IPBUDGETALLOCATOR_H
#define IPBUDGETALLOCATOR_H

#include <QHash>
#include <QSet>
#include "currency.h"
#include "daterange.h"
#include "itemprocessor.h"

class IPBudgetAllocator : public ItemProcessor
{
public:
   IPBudgetAllocator(QDate const& today);
   void finish();
   void processItem(LedgerAccount const& account);
   void processItem(LedgerBudget const& budget);
   void processItem(LedgerBudgetGoalEntry const& budget);
   void processItem(LedgerBudgetIncomeEntry const& budget);
   void processItem(LedgerBudgetReserveAmountEntry const& budget);
   void processItem(LedgerBudgetReservePercentEntry const& budget);
   void processItem(LedgerBudgetRoutineEntry const& budget);
   void processItem(LedgerReserve const& reserve);
   void processItem(LedgerReserveEntry const& reserve);
   void processItem(LedgerTransaction const& transaction);

private:
   void advanceBudgetPeriod(QString const& filename, uint lineNum,
                            QDate const& date, bool rebudgeting = false);
   void syncReserve(QString const& category);

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

   struct Routine
   {
      Currency currentAmount;
      Currency priorAmount;
      Currency reserved;
   };

private:
   QHash<QString, Currency> m_availables;
   DateRange m_currentPeriod;
   QHash<QString, Goal> m_goals;
   QSet<QString> m_incomes;
   QHash<QString, QString> m_owners;
   DateRange m_priorPeriod;
   QHash<QString, Reserve> m_reserves;
   QHash<QString, Routine> m_routines;
   bool m_singleReserve = false;
   QDate m_today;
};

#endif
