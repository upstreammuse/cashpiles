#pragma once

#include "m_currency.h"
#include "transactionentry.h"

namespace model { struct BudgetCategory; }
namespace model { struct TransactionCategoryEntry; }

struct model::TransactionCategoryEntry : public TransactionEntry
{
   std::shared_ptr<BudgetCategory const> category;  //FK
   Currency amount;

   TransactionCategoryEntry(
         std::shared_ptr<Transaction const>,
         std::shared_ptr<BudgetCategory const>);
};
