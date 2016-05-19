#include "account.h"

int Account::clearedBalance() const
{
   int total = 0;
   foreach (Txn const& txn, m_txns)
   {
      if (txn.cleared())
      {
         total += txn.amount();
      }
   }
   return total;
}

QString Account::name() const
{
   return m_name;
}

int Account::workingBalance() const
{
   int total = 0;
   foreach (Txn const& txn, m_txns)
   {
      total += txn.amount();
   }
   return total;
}

void Account::addTxn(const Txn &txn)
{
   m_txns.append(txn);
}
