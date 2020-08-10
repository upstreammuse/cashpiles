#pragma once

#include <memory>
#include "../currency.h"
#include "../util/m_date.h"
#include "modeldata.h"

namespace model { struct Account; }
namespace model { struct AccountStatement; }

struct model::AccountStatement : public ModelData
{
   int const id = newId();  //PK
   std::shared_ptr<Account const> account;  //FK
   Currency balance;
   util::Date date;

   AccountStatement(std::shared_ptr<Account const>, util::Date const&);
};
