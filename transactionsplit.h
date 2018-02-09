#ifndef TRANSACTIONSPLIT_H
#define TRANSACTIONSPLIT_H

#include <QString>

class TransactionSplit
{
public:
   TransactionSplit(QString const& payee, QString const& category,
                    QString const& memo, int amount, bool isTransfer = false);
   int amount() const;
   QString category() const;
   bool isTransfer() const;
   QString memo() const;
   QString payee() const;

private:
   int m_amount;
   QString m_category;
   bool m_isTransfer;
   QString m_memo;
   QString m_payee;
};

#endif
