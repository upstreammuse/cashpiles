#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <QDate>
#include <QList>
#include <QString>

#include "transactionsplit.h"

class Transaction
{
public:
   QString account() const;
   int amount() const;
   // TODO deprecate
   void appendSplit(const QString& payee, const QString& category,
                    const QString& memo, int amount);
   void appendSplit(const TransactionSplit& split);
   void clear();
   void setAccount(const QString& account);
   void setDate(const QDate& date);
   QList<TransactionSplit> splits() const;

   operator QString() const;

private:
   QString m_account;
   QList<int> m_amounts;
   QStringList m_categories;
   QDate m_date;
   QStringList m_memos;
   QStringList m_payees;
};

#endif
