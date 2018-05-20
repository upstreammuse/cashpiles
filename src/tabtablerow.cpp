#include "tabtablerow.h"

#include <QMap>

class TabTableRowPrivate : public QObject
{
public:
   TabTableRowPrivate(QObject* parent = 0);
   QMap<int, QString> columns;
};

TabTableRowPrivate::TabTableRowPrivate(QObject* parent)
   : QObject(parent)
{
}

TabTableRow::TabTableRow(QString const& line, QObject* parent)
   : QObject(parent)
   , d_ptr(new TabTableRowPrivate(this))
{
   Q_D(TabTableRow);

   QString column;
   int columnStart = 0;
   int index = 0;

   foreach (QChar c, line)
   {
      if (c == '\t')
      {
         if (column != "")
         {
            d->columns[columnStart] = column;
         }
         index += 8 - index % 8;
         columnStart = index;
         column.clear();
      }
      else
      {
         column.append(c);
         ++index;
      }
   }
   if (column != "")
   {
      d->columns[columnStart] = column;
   }
}

QList<int> TabTableRow::indices() const
{
   Q_D(const TabTableRow);
   return d->columns.keys();
}

QString TabTableRow::value(int index) const
{
   Q_D(const TabTableRow);
   Q_ASSERT(d->columns.contains(index));
   return d->columns.value(index);
}
