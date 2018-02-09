#include "accountant.h"

Accountant::Accountant(QObject* parent) :
   QObject(parent)
{
}

void Accountant::appendTransaction(Transaction const& transaction)
{
   m_accounts[transaction.account()].appendTransaction(transaction);
   foreach (TransactionSplit const& split, transaction.splits())
   {
      m_categories[split.category()].appendSplit(split);
   }
}

void Accountant::checkAccounts()
{
   foreach (Account const& account, m_accounts)
   {
      account.matchTransfers(m_accounts);
   }
}
