#pragma once

#include "../util/m_currency.h"
#include "transactionentry.h"

namespace model { struct AccountStatement; }
namespace model { struct BudgetAccount; }
namespace model { struct TransactionAccountEntry; }

struct model::TransactionAccountEntry : public TransactionEntry
{
   std::shared_ptr<BudgetAccount const> account;  //FK
   std::shared_ptr<AccountStatement const> statement;  //FK-nillable
   util::Currency amount;

   TransactionAccountEntry(
         std::shared_ptr<Transaction const>,
         std::shared_ptr<BudgetAccount const>);
};
