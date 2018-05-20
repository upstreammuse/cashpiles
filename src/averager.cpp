#include "averager.h"

Averager::Averager() :
   m_amount(0),
   m_firstDate(QDate::currentDate()),
   m_lastDate(QDate::currentDate())
{
}

int Averager::average() const
{
   return m_amount / (m_firstDate.daysTo(m_lastDate) + 1);
}

void Averager::addTxn(const Txn &txn)
{
   if (txn.date() < m_firstDate)
   {
      m_firstDate = txn.date();
   }
   if (txn.date() > m_lastDate)
   {
      m_lastDate = txn.date();
   }
   foreach (TxnPart const& part, txn.parts())
   {
      if (part.essential())
      {
         m_amount += part.amount();
      }
   }
}

void Averager::addTxns(const QList<Txn> &txns)
{
   foreach (Txn const& txn, txns)
   {
      addTxn(txn);
   }
}
