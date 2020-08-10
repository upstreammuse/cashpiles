#pragma once

#include "../currency.h"
#include "transactionentry.h"

namespace model { struct AccountStatement; }
namespace model { struct BudgetCategoryEntry; }
namespace model { struct ReferenceAccount; }
namespace model { struct TransactionCategoryTrackingEntry; }

struct model::TransactionCategoryTrackingEntry : public TransactionEntry
{
   std::shared_ptr<ReferenceAccount const> account;  //FK
   std::shared_ptr<BudgetCategoryEntry const> category;  //FK
   std::shared_ptr<AccountStatement const> statement;  //FK-nillable
   Currency amount;

   TransactionCategoryTrackingEntry(
         std::shared_ptr<Transaction const>,
         std::shared_ptr<ReferenceAccount const>,
         std::shared_ptr<BudgetCategoryEntry const>);
};
