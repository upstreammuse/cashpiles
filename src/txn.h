#ifndef TXN_H
#define TXN_H

#include <QDate>
#include <QList>
#include <QString>

class QDate;
class QString;

class TxnPart
{
public:
   int amount() const;
   bool cleared() const;
   bool essential() const;
   QString payee() const;
   QString pile() const;

   void setAmount(int amount);
   void setCleared(bool cleared);
   void setEssential(bool essential);
   void setPayee(QString const& payee);
   void setPile(QString const& pile);

private:
   // TODO use QPriv for these
   int m_amount;
   bool m_cleared;
   bool m_essential;
   QString m_payee;
   QString m_pile;
};

class Txn
{
public:
   Txn();
   int amount() const;
   bool cleared() const;
   QDate date() const;
   QList<TxnPart> parts() const;
   bool valid() const;

   void addPart(TxnPart const& part);
   void clear();
   void setAmount(int amount);
   void setCleared(bool cleared);
   void setDate(QDate const& date);

private:
   // TODO QPRIV
   int m_amount;
   bool m_cleared;
   QDate m_date;
   QList<TxnPart> m_parts;
};

#endif
