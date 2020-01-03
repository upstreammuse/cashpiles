#ifndef LEDGERBUDGETENTRY_H
#define LEDGERBUDGETENTRY_H

#include <QDate>
#include <QString>
#include "currency.h"
#include "interval.h"
#include "ledgeritem.h"

class ItemProcessor;

// TODO split these into separate files?

class LedgerBudgetEntry : public LedgerItem
{
public:
   QDate date() const;
   QString name() const;
   void setDate(QDate const& date);
   void setName(QString const& name);

private:
   QDate m_date;
   QString m_name;
};

class LedgerBudgetGoalEntry : public LedgerBudgetEntry
{
public:
   void processItem(ItemProcessor& processor) const;
};

class LedgerBudgetIncomeEntry : public LedgerBudgetEntry
{
public:
   void processItem(ItemProcessor& processor) const;
};

class LedgerBudgetReserveAmountEntry : public LedgerBudgetEntry
{
public:
   Currency amount() const;
   Interval interval() const;
   void processItem(ItemProcessor& processor) const;
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
   void processItem(ItemProcessor& processor) const;
   void setPercentage(uint percentage);

private:
   uint m_percentage;
};

class LedgerBudgetRoutineEntry : public LedgerBudgetEntry
{
public:
   void processItem(ItemProcessor& processor) const;
};

#endif
