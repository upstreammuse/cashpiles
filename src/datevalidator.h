#ifndef DATEVALIDATOR_H
#define DATEVALIDATOR_H

#include <QDate>
#include "itemprocessor.h"

class DateValidator : public ItemProcessor
{
public:
   void finish();
   void processItem(LedgerAccount const& account);
   void processItem(LedgerBudget const& budget);
   void processItem(LedgerBudgetGoalEntry const& entry);
   void processItem(LedgerBudgetIncomeEntry const& entry);
   void processItem(LedgerBudgetReserveAmountEntry const& entry);
   void processItem(LedgerBudgetReservePercentEntry const& entry);
   void processItem(LedgerBudgetRoutineEntry const& entry);
   void processItem(LedgerComment const& comment);
   void processItem(LedgerTransaction const& transaction);
   void start();

private:
   void processDate(QDate const& date, QString const& fileName, uint lineNum);

private:
   QDate m_latestDate;
};

#endif
