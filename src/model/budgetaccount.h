#pragma once

#include "account.h"

namespace model { struct BudgetAccount; }

struct model::BudgetAccount : public Account
{
   using Account::Account;
};
