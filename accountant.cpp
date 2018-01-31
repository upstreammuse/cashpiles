#include "accountant.h"

Accountant::Accountant(QObject* parent) :
   QObject(parent)
{
}

void Accountant::appendTransaction(Transaction const& transaction)
{
   m_accounts[transaction.account()].appendTransaction(transaction);

   // TODO this handles transfer balancing, budget category mods
   foreach (TransactionSplit const& split, transaction.splits())
   {
      m_categories[split.category()].appendSplit(split);
   }
}

/*
 * TODO
 * transaction needs a transfer flag on the payee to know it is an account name

foreach account, get list of transfer balances
foreach account in the transfer list, get its list of transfer balances
find the original acount in the second list, then compare values, should be opposite
*/
