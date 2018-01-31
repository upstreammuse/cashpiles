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

int Transaction::balance() const
{
   if (m_balance.empty())
   {
      return 0;
   }
   else
   {
      return m_balance.first();
   }
}

bool Transaction::cleared() const
{
   return m_cleared;
}

QDate Transaction::date() const
{
   return m_date;
}

bool Transaction::hasBalance() const
{
   return !m_balance.empty();
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

bool Transaction::operator<(Transaction const& other) const
{
   return m_date < other.m_date;
}
