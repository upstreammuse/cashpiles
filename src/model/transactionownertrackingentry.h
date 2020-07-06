#pragma once

#include "../util/m_currency.h"
#include "transactionentry.h"

namespace model { struct AccountStatement; }
namespace model { struct BudgetCategoryOwner; }
namespace model { struct ReferenceAccount; }
namespace model { struct TransactionOwnerTrackingEntry; }

struct model::TransactionOwnerTrackingEntry : public TransactionEntry
{
   std::shared_ptr<ReferenceAccount const> account;  //FK
   std::shared_ptr<BudgetCategoryOwner const> owner;  //FK
   std::shared_ptr<AccountStatement const> statement;  //FK-nillable
   util::Currency amount;

   TransactionOwnerTrackingEntry(
         std::shared_ptr<Transaction const>,
         std::shared_ptr<ReferenceAccount const>,
         std::shared_ptr<BudgetCategoryOwner const>);
};
