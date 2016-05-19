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
   QString account = rx.cap(1);
   qDebug() << "account is " << account;

   DataTableRow* header = data->header();
   QStringList keys = header->keys();
   Q_ASSERT(keys.contains("Date"));
   Q_ASSERT(keys.contains("Payee"));
   Q_ASSERT(keys.contains("Amount"));
   Q_ASSERT(keys.contains("Essential"));

   Txn txn;
   for (int i = 0; i < data->rows(); ++i)
   {
      qDebug() << i;
      DataTableRow* row = data->row(i);

      // read row data
      QDate date;
      if (row->keys().contains("Date"))
      {
         date = QDate::fromString(row->value("Date"), "M/d/yy");
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
         accounts[account].addTxn(txn);
         txn.clear();
         txn.setDate(date);
         txn.setAmount(amount);
         txn.setCleared(cleared);
      }

      // if payee signals a split, make sure this was starting a new txn, then
      // populate txn with validation data
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

         // add to transfer pool if it is a transfer
         if (payee.startsWith("@"))
         {
            transferPool.addTransfer(amount);
         }

         txn.addPart(txnPart);
      }
   }

   // finalize last txn
   Q_ASSERT(txn.valid());
   accounts[account].addTxn(txn);
}




      // TODO save into essentials average if essential
      // TODO push into the statements container if cleared

      // TODO statements container is sorted list of buckets with date properties
      // - finding a bucket is the first with a date equal or after the transaction date


/*
to build essential spending reserve, do something like this

- calculate average of essential costs ($/day)
- starting with first day (earliest xaction), add average to reserve
- when encountering an essential xaction without a pile, subtract from reserve
  - if reserve ever becomes negative, add to reserve until 0
- iterate for all days to present

After doing this, you have the breakdown

- total cash available
  - (include cleared income and all expenses across all accounts)
- subtract value of uncategorized essentials pile
- subtract pile reservations
- subtract emergency fund reservation
- what's left is spendable



How to manage interplay between essentials and piles?
- essential may have a pile, how does this calculate?

*/
