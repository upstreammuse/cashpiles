#ifndef MODEL_ACCOUNTBALANCE_H
#define MODEL_ACCOUNTBALANCE_H

#include <QString>
#include "currency.h"

namespace model { struct AccountBalance; }

struct model::AccountBalance
{
   QString account;  //PK,FK
   Currency balance;
};

#endif
