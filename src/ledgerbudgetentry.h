#ifndef LEDGERBUDGETENTRY_H
#define LEDGERBUDGETENTRY_H

#include <QDate>
#include <QString>
#include "currency.h"
#include "interval.h"

class ItemProcessor;

// TODO split these into separate files?

class LedgerBudgetEntry
{
public:
   virtual ~LedgerBudgetEntry();
   QDate date() const;
   QString name() const;
   virtual void processItem(ItemProcessor& processor) = 0;
   void setDate(QDate const& date);
   void setName(QString const& name);

private:
   QDate m_date;
   QString m_name;
};

class LedgerBudgetGoalEntry : public LedgerBudgetEntry
{
public:
   void processItem(ItemProcessor& processor);
};

class LedgerBudgetIncomeEntry : public LedgerBudgetEntry
{
public:
   void processItem(ItemProcessor& processor);
};

class LedgerBudgetReserveAmountEntry : public LedgerBudgetEntry
{
public:
   Currency amount() const;
   Interval interval() const;
   void processItem(ItemProcessor& processor);
   void setAmount(Currency const& amount);
   void setInterval(Interval const& interval);

private:
   Currency m_amount;
   Interval m_interval;
};

class LedgerBudgetReservePercentEntry : public LedgerBudgetEntry
{
public:
   uint percentage() const;
   void processItem(ItemProcessor& processor);
   void setPercentage(uint percentage);

private:
   uint m_percentage;
};

class LedgerBudgetRoutineEntry : public LedgerBudgetEntry
{
public:
   void processItem(ItemProcessor& processor);
};

#endif
