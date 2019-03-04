#ifndef ACCOUNTBALANCE_H
#define ACCOUNTBALANCE_H

#include <QString>
#include "currency.h"

namespace model { struct AccountBalance; }

struct model::AccountBalance
{
   QString account;  //PK,FK
   Currency balance;
};

#endif
