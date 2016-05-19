#include "datatablefactory.h"

#include "datatable.h"
#include "datatablerow.h"
#include "tabtable.h"
#include "tabtablerow.h"

#include <QSharedPointer>

DataTable* DataTableFactory::create(TabTable* tt)
{
   QScopedPointer<DataTable> dt(new DataTable);
   dt->setTitle(tt->title());

   TabTableRow* tabHeader = tt->header();
   DataTableRow* dataHeader = new DataTableRow(this);
   foreach (int index, tabHeader->indices())
   {
      dataHeader->set(tabHeader->value(index), "");
   }
   dt->setHeader(dataHeader);

   for (int i = 0; i < tt->rows(); ++i)
   {
      TabTableRow* tabRow = tt->row(i);
      DataTableRow* dataRow = new DataTableRow(this);
      foreach (int index, tabRow->indices())
      {
         dataRow->set(tabHeader->value(index), tabRow->value(index));
      }
      dt->addRow(dataRow);
   }

   return dt.take();
}
