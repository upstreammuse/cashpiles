#pragma once

#include <memory>
#include "m_currency.h"
#include "m_date.h"
#include "modeldata.h"

namespace model { struct Account; }
namespace model { struct AccountStatement; }

struct model::AccountStatement : public ModelData
{
   int const id = newId();  //PK
   std::shared_ptr<Account> account;  //FK
   Currency balance;
   Date date;

   AccountStatement(std::shared_ptr<Account>, Date const&);
};
