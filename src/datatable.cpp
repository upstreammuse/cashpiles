#include "datatable.h"

#include "datatablerow.h"

#include <QStringList>

class DataTablePrivate : public QObject
{
public:
   DataTablePrivate(QObject* parent = 0);
   QList<DataTableRow*> data;
   QString title;
};

DataTablePrivate::DataTablePrivate(QObject* parent)
   : QObject(parent)
{
}

DataTable::DataTable(QObject* parent)
   : QObject(parent)
   , d_ptr(new DataTablePrivate(this))
{
}

void DataTable::addRow(DataTableRow* row)
{
   Q_D(DataTable);
   Q_ASSERT(d->data.size() > 0);
   foreach (QString const& key, row->keys())
   {
      Q_ASSERT(d->data.at(0)->keys().contains(key));
   }
   d->data.append(row);
   row->setParent(this);
}

void DataTable::setHeader(DataTableRow* header)
{
   Q_D(DataTable);
   Q_ASSERT(d->data.size() == 0);
   d->data.append(header);
   header->setParent(this);
}

void DataTable::setTitle(QString const& title)
{
   Q_D(DataTable);
   d->title = title;
}

DataTableRow* DataTable::header() const
{
   Q_D(const DataTable);
   Q_ASSERT(d->data.size() > 0);
   return d->data.at(0);
}

DataTableRow* DataTable::row(int index) const
{
   Q_D(const DataTable);
   Q_ASSERT(d->data.size() > index + 1);
   return d->data.at(index + 1);
}

int DataTable::rows() const
{
   Q_D(const DataTable);
   Q_ASSERT(d->data.size() > 0);
   return d->data.size() - 1;
}

QString DataTable::title() const
{
   Q_D(const DataTable);
   return d->title;
}
