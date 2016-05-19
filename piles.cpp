#include "datatable.h"
#include "datatablefactory.h"
#include "tabtable.h"
#include "txntable.h"

#include <QSharedPointer>
#include <QTextStream>

int main()
{
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
