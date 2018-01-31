#ifndef CATEGORY_H
#define CATEGORY_H

#include "transactionsplit.h"

class Category
{
public:
   void appendSplit(TransactionSplit const& split);
};

#endif
