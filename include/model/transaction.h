#ifndef MODEL_TRANSACTION_H
#define MODEL_TRANSACTION_H

#include <QString>

namespace model { struct Transaction; }

struct model::Transaction
{
   int id;  //PK
   QString account;  //FK
   bool cleared;
};

#endif
