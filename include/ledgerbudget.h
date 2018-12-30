#ifndef LEDGERBUDGET_H
#define LEDGERBUDGET_H

#include <QDate>
#include <QSet>
#include "currency.h"
#include "interval.h"
#include "ledgeritem.h"

class LedgerBudget : public LedgerItem
{
public:
   // TODO rework this to not have such a "union" design to its members, or
   // use methods to enforce integrity of fields w.r.t. type.  Or have some kind
   // of inheritance, which might be too much?
   struct Category
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

public:
   LedgerBudget(QString const& fileName, int lineNum);

   QHash<QString, Category> categories() const;
   void insertCategory(QString const& name, Category const& category);

   QDate date() const;
   void setDate(QDate const& date);

   Interval interval() const;
   void setInterval(Interval interval);

   void processItem(ItemProcessor* processor);

private:
   QHash<QString, Category> m_categories;
   QDate m_date;
   Interval m_interval;
};

#endif
