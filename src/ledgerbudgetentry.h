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

#endif
