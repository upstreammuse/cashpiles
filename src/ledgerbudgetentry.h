#ifndef LEDGERBUDGETENTRY_H
#define LEDGERBUDGETENTRY_H

#include <QDate>
#include <QString>
#include "currency.h"
#include "identifier.h"
#include "interval.h"
#include "ledgeritem.h"

class ItemProcessor;

class LedgerBudgetEntry : public LedgerItem
{
public:
   LedgerBudgetEntry(QString const& filename, uint lineNum);

   Identifier category() const;
   void setCategory(Identifier const& category);

   QDate date() const;
   void setDate(QDate const& date);

   Identifier owner() const;
   void setOwner(Identifier const& owner);

private:
   Identifier m_category;
   QDate m_date;
   Identifier m_owner;
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
   void setAmount(Currency const& amount);

   Interval interval() const;
   void setInterval(Interval const& interval);

   void processItem(ItemProcessor& processor) const;

private:
   Currency m_amount;
   Interval m_interval;
};

class LedgerBudgetReservePercentEntry : public LedgerBudgetEntry
{
public:
   LedgerBudgetReservePercentEntry(QString const& filename, uint lineNum);

   uint percentage() const;
   void setPercentage(uint percentage);

   void processItem(ItemProcessor& processor) const;

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
