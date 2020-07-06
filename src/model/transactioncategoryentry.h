#pragma once

#include "../util/m_currency.h"
#include "transactionentry.h"

namespace model { struct BudgetCategoryEntry; }
namespace model { struct TransactionCategoryEntry; }

struct model::TransactionCategoryEntry : public TransactionEntry
{
   std::shared_ptr<BudgetCategoryEntry const> category;  //FK
   util::Currency amount;

   TransactionCategoryEntry(
         std::shared_ptr<Transaction const>,
         std::shared_ptr<BudgetCategoryEntry const>);
};
