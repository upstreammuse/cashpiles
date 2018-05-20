#ifndef AVERAGER_H
#define AVERAGER_H

#include "txn.h"

class Averager
{
public:
   Averager();
   int average() const;

   void addTxn(Txn const& txn);
   void addTxns(QList<Txn> const& txns);

private:
   // TODO qpriv
   int m_amount;
   QDate m_firstDate;
   QDate m_lastDate;
};

#endif
