#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <QDate>
#include <QList>
#include "transactionsplit.h"

class Transaction
{
public:
   Transaction();
   QString account() const;
   int amount() const;
   void appendSplit(TransactionSplit const& split);
   int balance() const;
   bool cleared() const;
   QDate date() const;
   bool hasBalance() const;
   void setAccount(QString const& account);
   void setCleared(bool cleared);
   void setDate(QDate const& date);
   QList<TransactionSplit> splits() const;

   bool operator<(Transaction const& other) const;

private:
   QString m_account;
   QList<int> m_balance;
   bool m_cleared;
   QDate m_date;
   QList<TransactionSplit> m_splits;
};

#endif
