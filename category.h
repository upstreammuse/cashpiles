#ifndef CATEGORY_H
#define CATEGORY_H

#include "transactionsplit.h"

class Category
{
public:
   Category();
   void appendSplit(TransactionSplit const& split);

private:
   void verifyBalance();
   void verifyName(QString const& name);

private:
   int m_balance;
   QString m_name;
};

#endif
