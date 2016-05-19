#include "tabtable.h"

#include "tabtablerow.h"

#include <QTextStream>

class TabTablePrivate : public QObject
{
public:
   TabTablePrivate(QObject* parent = 0);
   QList<TabTableRow*> data;
   QString title;
};

TabTablePrivate::TabTablePrivate(QObject* parent)
   : QObject(parent)
{
}

TabTable::TabTable(QObject* parent)
   : QObject(parent)
   , d_ptr(new TabTablePrivate(this))
{
}

void TabTable::addRow(TabTableRow* row)
{
   Q_D(TabTable);
   Q_ASSERT(d->data.size() > 0);
   foreach (int index, row->indices())
   {
      Q_ASSERT(d->data.at(0)->indices().contains(index));
   }
   d->data.append(row);
   row->setParent(this);
}

void TabTable::setHeader(TabTableRow* header)
{
   Q_D(TabTable);
   Q_ASSERT(d->data.size() == 0);
   d->data.append(header);
   header->setParent(this);
}

void TabTable::setTitle(QString const& title)
{
   Q_D(TabTable);
   d->title = title;
}

TabTableRow* TabTable::header() const
{
   Q_D(const TabTable);
   Q_ASSERT(d->data.size() > 0);
   return d->data.at(0);
}

TabTableRow* TabTable::row(int index) const
{
   Q_D(const TabTable);
   Q_ASSERT(d->data.size() > index + 1);
   return d->data.at(index + 1);
}

int TabTable::rows() const
{
   Q_D(const TabTable);
   Q_ASSERT(d->data.size() > 0);
   return d->data.size() - 1;
}

QString TabTable::title() const
{
   Q_D(const TabTable);
   return d->title;
}

QTextStream& operator>>(QTextStream& in, TabTable& tt)
{
   QString line;

   while ((line = in.readLine()) == "") ;
   tt.setTitle(line);

   line = in.readLine();
   TabTableRow* row = new TabTableRow(line, &tt);
   tt.setHeader(row);

   while ((line = in.readLine()) != "")
   {
      TabTableRow* row = new TabTableRow(line, &tt);
      tt.addRow(row);
   }

   return in;
}
