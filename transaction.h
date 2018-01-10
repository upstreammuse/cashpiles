#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <QDate>
#include <QList>
#include "transactionsplit.h"

class Transaction
{
public:
   QString account() const;
   int amount() const;
   void appendSplit(TransactionSplit const& split);
   void clear();
   void setAccount(QString const& account);
   void setDate(QDate const& date);
   QList<TransactionSplit> splits() const;

private:
   QString m_account;
   QDate m_date;
   QList<TransactionSplit> m_splits;
};

#endif
