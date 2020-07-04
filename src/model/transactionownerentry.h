#pragma once

#include "m_currency.h"
#include "transactionentry.h"

namespace model { struct BudgetCategoryOwner; }
namespace model { struct TransactionOwnerEntry; }

struct model::TransactionOwnerEntry : public TransactionEntry
{
   std::shared_ptr<BudgetCategoryOwner const> owner;  //FK
   Currency amount;

   TransactionOwnerEntry(
         std::shared_ptr<Transaction const>,
         std::shared_ptr<BudgetCategoryOwner const>);
};
