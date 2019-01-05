#ifndef LEDGERACCOUNTBALANCE_H
#define LEDGERACCOUNTBALANCE_H

#include <QDate>
#include "currency.h"
#include "ledgeritem.h"

class LedgerAccountBalance : public LedgerItem
{
public:
   LedgerAccountBalance();
   LedgerAccountBalance(QString const& fileName, int lineNum);

   QString account() const;
   Currency amount() const;
   QDate date() const;

   void setAccount(QString const& account);
   void setAmount(Currency const& amount);
   void setDate(QDate const& date);

   void processItem(ItemProcessor* processor);

private:
   QString m_account;
   Currency m_amount;
   QDate m_date;
};

#endif
