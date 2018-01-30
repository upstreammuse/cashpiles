#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <QDate>
#include <QList>
#include "transactionsplit.h"

/* TODO accounts can have a reconciliation date and a value that means that all
 * transactions before that date have to be cleared, and that they will add up
 * to the reconciled value
 */

class Transaction
{
public:
   Transaction();
   QString account() const;
   int amount() const;
   void appendSplit(TransactionSplit const& split);
   bool cleared() const;
   void setAccount(QString const& account);
   void setCleared(bool cleared);
   void setDate(QDate const& date);
   QList<TransactionSplit> splits() const;

private:
   QString m_account;
   bool m_cleared;
   QDate m_date;
   QList<TransactionSplit> m_splits;
};

#endif
