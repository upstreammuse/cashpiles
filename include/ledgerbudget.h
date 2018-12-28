#ifndef LEDGERBUDGET_H
#define LEDGERBUDGET_H

#include <QDate>
#include <QSet>
#include "currency.h"
#include "interval.h"
#include "ledgeritem.h"

// TODO this is awful, but will do the trick for right now
struct BudgetCategory
{
   enum class Type
   {
      GOAL,
      INCOME,
      RESERVE_AMOUNT,
      RESERVE_PERCENT,
      ROUTINE
   };
   Type type;
   Currency amount;
   Interval interval;
   int percentage;
};

class LedgerBudget : public LedgerItem
{
public:
   LedgerBudget(QString const& fileName, int lineNum);

   QHash<QString, BudgetCategory> categories() const;
   void insertCategory(QString const& category, BudgetCategory const& data);

   QDate date() const;
   void setDate(QDate const& date);

   Interval interval() const;
   void setInterval(Interval interval);

   void processItem(ItemProcessor* processor);

private:
   QHash<QString, BudgetCategory> m_categories;
   QDate m_date;
   Interval m_interval;
};

#endif
