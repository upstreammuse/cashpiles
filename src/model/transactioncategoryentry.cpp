#include "transactioncategoryentry.h"

using namespace model;

TransactionCategoryEntry::TransactionCategoryEntry(
      std::shared_ptr<Transaction const> transaction,
      std::shared_ptr<BudgetCategory const> category_) :
   TransactionEntry(transaction),
   category(category_)
{
}
