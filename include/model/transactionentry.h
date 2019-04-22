#ifndef MODEL_TRANSACTIONENTRY_H
#define MODEL_TRANSACTIONENTRY_H

#include <QString>
#include "currency.h"

namespace model { struct CategorizedTransactionEntry; }
namespace model { struct CategorizedTransferTransactionEntry; }
namespace model { struct UncategorizedTransactionEntry; }
namespace model { struct UncategorizedTransferTransactionEntry; }

struct model::CategorizedTransactionEntry
{
   int id;  //PK
   int transaction;  //FK
   QString payee;
   QString category;  //FK
   Currency amount;
};

struct model::CategorizedTransferTransactionEntry
{
   int id;  //PK
   int transaction1;  //FK
   int transaction2;  //FK
   QString category;  //FK
   Currency amount;
};

struct model::UncategorizedTransactionEntry
{
   int id;  //PK
   int transaction;  //FK
   QString payee;
   Currency amount;
};

struct model::UncategorizedTransferTransactionEntry
{
   int id;  //PK
   int transaction1;  //FK
   int transaction2;  //FK
   Currency amount;
};

#endif
