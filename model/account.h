#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <QString>

namespace model { struct Account; }

struct model::Account
{
   QString name;  //PK
   bool onBudget;
};

#endif
