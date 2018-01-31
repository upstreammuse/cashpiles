#ifndef ACCOUNT_H
#define ACCOUNT_H

#include "transaction.h"

class Account
{
public:
   Account();
   void appendTransaction(Transaction const& transaction);

private:
   void verifyBalance(int balance, bool hasBalance);
   void verifyDate(QDate const& date);
   void verifyName(QString const& name);

private:
   bool m_allClear;
   int m_balance;
   QDate m_latestDate;
   QString m_name;
};

#endif
