#include "transactionaccountentry.h"

using namespace model;

TransactionAccountEntry::TransactionAccountEntry(
      std::shared_ptr<Transaction const> transaction,
      std::shared_ptr<BudgetAccount const> account_) :
   TransactionEntry(transaction),
   account(account_)
{
}
