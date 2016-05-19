#include "txn.h"

Txn::Txn() :
   m_amount(0),
   m_cleared(false)
{
}

int TxnPart::amount() const
{
   return m_amount;
}

bool TxnPart::cleared() const
{
   return m_cleared;
}

bool TxnPart::essential() const
{
   return m_essential;
}

QString TxnPart::payee() const
{
   return m_payee;
}

QString TxnPart::pile() const
{
   return m_pile;
}

void TxnPart::setAmount(int amount)
{
   m_amount = amount;
}

void TxnPart::setCleared(bool cleared)
{
   m_cleared = cleared;
}

void TxnPart::setEssential(bool essential)
{
   m_essential = essential;
}

void TxnPart::setPayee(const QString &payee)
{
   m_payee = payee;
}

void TxnPart::setPile(const QString &pile)
{
   m_pile = pile;
}

int Txn::amount() const
{
   return m_amount;
}

bool Txn::cleared() const
{
   return m_cleared;
}

QDate Txn::date() const
{
   return m_date;
}

QList<TxnPart> Txn::parts() const
{
   return m_parts;
}

bool Txn::valid() const
{
   int total = 0;
   foreach (TxnPart const& part, m_parts)
   {
      if (part.cleared() != m_cleared)
      {
         return false;
      }
      total += part.amount();
   }
   if (total != m_amount)
   {
      return false;
   }
   return true;
}

void Txn::addPart(const TxnPart &part)
{
   Q_ASSERT(part.cleared() == m_cleared);
   m_parts.append(part);
}

void Txn::clear()
{
   m_amount = 0;
   m_cleared = false;
   m_date = QDate();
   m_parts.clear();
}

void Txn::setAmount(int amount)
{
   m_amount = amount;
}

void Txn::setCleared(bool cleared)
{
   m_cleared = cleared;
}

void Txn::setDate(const QDate &date)
{
   Q_ASSERT(date.isValid());
   m_date = date;
}
