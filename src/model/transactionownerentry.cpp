#include "transactionownerentry.h"

using namespace model;

TransactionOwnerEntry::TransactionOwnerEntry(
      std::shared_ptr<Transaction const> transaction,
      std::shared_ptr<BudgetCategoryOwner const> owner_) :
   TransactionEntry(transaction),
   owner(owner_)
{
}
