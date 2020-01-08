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
   LedgerBudgetEntry(QString const& filename, uint lineNum);
   QDate date() const;
   QString name() const;
   QString owner() const;
   void setDate(QDate const& date);
   void setName(QString const& name);
   void setOwner(QString const& owner);

private:
   QDate m_date;
   QString m_name;
   QString m_owner;
};

class LedgerBudgetGoalEntry : public LedgerBudgetEntry
{
public:
   LedgerBudgetGoalEntry(QString const& filename, uint lineNum);
   void processItem(ItemProcessor& processor) const;
};

class LedgerBudgetIncomeEntry : public LedgerBudgetEntry
{
public:
   LedgerBudgetIncomeEntry(QString const& filename, uint lineNum);
   void processItem(ItemProcessor& processor) const;
};

class LedgerBudgetReserveAmountEntry : public LedgerBudgetEntry
{
public:
   LedgerBudgetReserveAmountEntry(QString const& filename, uint lineNum);
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
   LedgerBudgetReservePercentEntry(QString const& filename, uint lineNum);
   uint percentage() const;
   void processItem(ItemProcessor& processor) const;
   void setPercentage(uint percentage);

private:
   uint m_percentage;
};

class LedgerBudgetRoutineEntry : public LedgerBudgetEntry
{
public:
   LedgerBudgetRoutineEntry(QString const& filename, uint lineNum);
   void processItem(ItemProcessor& processor) const;
};

#endif
