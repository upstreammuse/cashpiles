#ifndef TRANSFERBALANCER_H
#define TRANSFERBALANCER_H

#include <QDate>
#include <QHash>
#include <QSet>
#include "currency.h"
#include "identifier.h"
#include "itemprocessor.h"

class TransferBalancer : public ItemProcessor
{
public:
   void finish();
   void processItem(LedgerAccount const& account);
   void processItem(LedgerBudget const& budget);
   void processItem(LedgerReserve const& reserve);
   void processItem(LedgerTransaction const& transaction);
   void setDateFormat(QString const& dateFormat);

private:
   void checkTransfers(QDate const& date);

private:
   QSet<Identifier> m_accounts;
   QString m_dateFormat = "yyyy/MM/dd";
   QDate m_lastDate;
   QHash<Identifier, QHash<Identifier, Currency>> m_transfers;
};

#endif
