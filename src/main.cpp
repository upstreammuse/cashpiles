#include "averager.h"
#include "datatable.h"
#include "datatablefactory.h"
#include "tabtable.h"
#include "txntable.h"

#include <QSharedPointer>
#include <QTextStream>

#include <QDebug>
int main()
{
   QMap<QString, Account> accounts;
   int balance = 0;
   int transferBalance = 0;
   Averager avg;

   QScopedPointer<DataTableFactory> factory(new DataTableFactory);
   QTextStream in(stdin);

   while (!in.atEnd())
   {
      QScopedPointer<TabTable> tt(new TabTable);
      in >> *tt;
      QScopedPointer<DataTable> dt(factory->create(tt.data()));

      if (dt->title().startsWith("Transactions"))
      {
         QScopedPointer<TxnTable> txnTable(new TxnTable(dt.data()));
         Account& account = accounts[txnTable->account()];

         account.addTxns(txnTable->txns());
         balance += account.workingBalance();
         qDebug() << "working balance" << balance;
         transferBalance += account.transferBalance();
         qDebug() << "transfer balance" << transferBalance;
         avg.addTxns(txnTable->txns());
         qDebug() << "average essential" << avg.average();
      }
      else if (dt->title().startsWith("Statements"))
      {
         // TODO
      }
      else if (dt->title().startsWith("Piles"))
      {
         // TODO
      }
   }
   return 0;
}
