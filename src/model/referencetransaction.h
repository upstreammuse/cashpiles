#pragma once

#include <memory>
#include "../currency.h"
#include "../date.h"
#include "modeldata.h"

namespace model { struct AccountStatement; }
namespace model { struct ReferenceAccount; }
namespace model { struct ReferenceTransaction; }
namespace model { enum class TransactionFlag; }

struct model::ReferenceTransaction : public ModelData
{
   int const id = newId();  //PK
   std::shared_ptr<ReferenceAccount const> account;  //FK
   std::shared_ptr<AccountStatement const> statement;  //FK-nillable
   Currency amount;
   Date date;
   TransactionFlag flag;
   std::string payee;

   ReferenceTransaction(
         std::shared_ptr<ReferenceAccount const>, Date const&,
         TransactionFlag, std::string const& payee);
};
