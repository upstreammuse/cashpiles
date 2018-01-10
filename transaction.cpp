#include "transaction.h"

QString Transaction::account() const
{
   return m_account;
}

int Transaction::amount() const
{
   int sum = 0;
   foreach (TransactionSplit const& split, m_splits)
   {
      sum += split.amount();
   }
   return sum;
}

void Transaction::appendSplit(TransactionSplit const& split)
{
   m_splits.append(split);
}

void Transaction::clear()
{
   m_account.clear();
   m_date = QDate();
   m_splits.clear();
}

void Transaction::setAccount(QString const& account)
{
   m_account = account;
}

void Transaction::setDate(QDate const& date)
{
   m_date = date;
}

QList<TransactionSplit> Transaction::splits() const
{
   return m_splits;
}
