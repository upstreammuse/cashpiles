#include "txntable.h"

#include "account.h"
#include "datatable.h"
#include "datatablerow.h"
#include "txn.h"

#include <QDate>
#include <QStringList>

// TODO these only work for currencies with "cents"

int toCurrency(double x)
{
   return qRound(x * 100);
}

int toCurrency(QString s)
{
   return toCurrency(s.toDouble());
}

#include <QDebug>
TxnTable::TxnTable(DataTable* data, QObject* parent)
   : QObject(parent)
{
   QRegExp rx("Transactions \\((.*)\\)");
   Q_ASSERT(rx.exactMatch(data->title()));
   m_account = rx.cap(1);
   qDebug() << "account is " << m_account;

   DataTableRow* header = data->header();
   QStringList keys = header->keys();
   Q_ASSERT(keys.contains("Date"));
   Q_ASSERT(keys.contains("Payee"));
   Q_ASSERT(keys.contains("Amount"));
   Q_ASSERT(keys.contains("Essential"));

   Txn txn;
   for (int i = 0; i < data->rows(); ++i)
   {
      DataTableRow* row = data->row(i);

      // read row data
      QDate date;
      if (row->keys().contains("Date"))
      {
         date = QDate::fromString(row->value("Date"), "M/d/yy");
         while (date.year() < 1980)
         {
            date = date.addYears(100);
         }
         qDebug() << row->value(("Date"));
         qDebug() << date;
         Q_ASSERT(date.isValid());
      }
      QString payee = row->value("Payee");
      int amount = toCurrency(row->value("Amount"));
      QString pile;
      if (row->keys().contains("Pile"))
      {
         pile = row->value("Pile");
      }
      bool essential = row->keys().contains("Essential");
      bool cleared = row->keys().contains("Cleared");

      // if date set, new txn, otherwise adding a part to previous txn
      if (date.isValid())
      {
         Q_ASSERT(txn.valid());
         m_txns.append(txn);
         txn.clear();
         txn.setDate(date);
         txn.setAmount(amount);
         txn.setCleared(cleared);
      }

      // if payee signals a split, make sure it follows split rules
      if (payee == "--split--")
      {
         Q_ASSERT(date.isValid());
         Q_ASSERT(pile == "");
         Q_ASSERT(!essential);
      }
      // otherwise populate txn with new part
      else
      {
         TxnPart txnPart;
         txnPart.setPayee(payee);
         txnPart.setAmount(amount);
         txnPart.setPile(pile);
         txnPart.setEssential(essential);
         txnPart.setCleared(cleared);
         txn.addPart(txnPart);
      }
   }

   // finalize last txn
   Q_ASSERT(txn.valid());
   m_txns.append(txn);
}

QString TxnTable::account() const
{
   return m_account;
}

QList<Txn> TxnTable::txns() const
{
   return m_txns;
}
