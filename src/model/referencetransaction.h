#pragma once

#include <memory>
#include "m_currency.h"
#include "m_date.h"
#include "modeldata.h"

namespace model { struct AccountStatement; }
namespace model { struct ReferenceAccount; }
namespace model { struct ReferenceTransaction; }
namespace model { enum class TransactionFlag; }

struct model::ReferenceTransaction : public ModelData
{
   int const id = newId();  //PK
   std::shared_ptr<ReferenceAccount> account;  //FK
   std::shared_ptr<AccountStatement> statement;  //FK-nillable
   Currency amount;
   Date date;
   std::string payee;
   TransactionFlag status;

   ReferenceTransaction(
         std::shared_ptr<ReferenceAccount>, Date const&, std::string const&,
         TransactionFlag);
};
