#pragma once

#include "account.h"

namespace model { struct ReferenceAccount; }

struct model::ReferenceAccount : public Account
{
   using Account::Account;
};
