#ifndef ACCOUNT_H
#define ACCOUNT_H

class QString;

#include "txn.h"

class Account
{
public:
   int clearedBalance() const;
   QString name() const;
   int workingBalance() const;

   void addTxn(Txn const& txn);

private:
   // TODO QPRIV
   QString m_name;
   QList<Txn> m_txns;
};

#endif
