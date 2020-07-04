#include "transactionownertrackingentry.h"

using namespace model;

TransactionOwnerTrackingEntry::TransactionOwnerTrackingEntry(
      std::shared_ptr<Transaction const> transaction,
      std::shared_ptr<ReferenceAccount const> account_,
      std::shared_ptr<BudgetCategoryOwner const> owner_) :
   TransactionEntry(transaction),
   account(account_),
   owner(owner_)
{
}
