#include "transactioncategorytrackingentry.h"

using namespace model;

TransactionCategoryTrackingEntry::TransactionCategoryTrackingEntry(
      std::shared_ptr<Transaction const> transaction,
      std::shared_ptr<ReferenceAccount const> account_,
      std::shared_ptr<BudgetCategory const> category_) :
   TransactionEntry(transaction),
   account(account_),
   category(category_)
{
}
