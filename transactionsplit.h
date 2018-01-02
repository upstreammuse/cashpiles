#ifndef TRANSACTIONSPLIT_H
#define TRANSACTIONSPLIT_H

#include <QString>

class TransactionSplit
{
public:
   TransactionSplit();
   QString payee;
   QString category;
   QString memo;
   int amount;
};

#endif // TRANSACTIONSPLIT_H
