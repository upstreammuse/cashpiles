#pragma once

#include <memory>
#include "account.h"
#include "modeldata.h"

namespace model { struct AccountClosure; }

struct model::AccountClosure : public ModelData
{
   std::shared_ptr<Account const> const account;  //PK, FK

   explicit AccountClosure(std::shared_ptr<Account const>);
};
