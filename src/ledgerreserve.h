#ifndef LEDGERRESERVE_H
#define LEDGERRESERVE_H

#include <QDate>
#include "currency.h"
#include "ledgeritem.h"

class LedgerReserve : public LedgerItem
{
public:
   LedgerReserve(QString const& filename, uint linenum);
   Currency amount() const;
   QString category() const;
   QDate date() const;
   void processItem(ItemProcessor& processor) const;
   void setAmount(Currency const& amount);
   void setCategory(QString const& category);
   void setDate(QDate const& date);

private:
   Currency m_amount;
   QString m_category;
   QDate m_date;
};

#endif
