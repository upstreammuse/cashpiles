#ifndef LEDGERBUDGET_H
#define LEDGERBUDGET_H

#include <QDate>
#include <QSet>
#include "ledgeritem.h"

class LedgerBudget : public LedgerItem
{
public:
   LedgerBudget(QString const& fileName, int lineNum);

   QSet<QString> categories() const;
   void addCategory(QString const& category);

   QDate date() const;
   void setDate(QDate const& date);

   void processItem(ItemProcessor* processor);

private:
   QSet<QString> m_categories;
   QDate m_date;
};

#endif

/* TODO
 * names of routine categories
 * names of goal categories
 * names of reserve% categories with percentage
 * names of fixed reserve categories with amount and interval
 */
