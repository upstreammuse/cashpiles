#include "transaction.h"

Transaction::Transaction() :
   m_cleared(false)
{
}

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

bool Transaction::cleared() const
{
   return m_cleared;
}

void Transaction::setAccount(QString const& account)
{
   m_account = account;
}

void Transaction::setCleared(bool cleared)
{
   m_cleared = cleared;
}

void Transaction::setDate(QDate const& date)
{
   m_date = date;
}

QList<TransactionSplit> Transaction::splits() const
{
   return m_splits;
}
