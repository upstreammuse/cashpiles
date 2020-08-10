#pragma once

#include "../currency.h"
#include "transactionentry.h"

namespace model { struct BudgetCategoryEntry; }
namespace model { struct TransactionCategoryEntry; }

struct model::TransactionCategoryEntry : public TransactionEntry
{
   std::shared_ptr<BudgetCategoryEntry const> category;  //FK
   Currency amount;

   TransactionCategoryEntry(
         std::shared_ptr<Transaction const>,
         std::shared_ptr<BudgetCategoryEntry const>);
};
