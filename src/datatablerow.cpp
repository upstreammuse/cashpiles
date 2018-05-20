#include "datatablerow.h"

#include <QMap>
#include <QStringList>

class DataTableRowPrivate : public QObject
{
public:
   DataTableRowPrivate(QObject* parent = 0);
   QMap<QString, QString> columns;
};

DataTableRowPrivate::DataTableRowPrivate(QObject* parent)
   : QObject(parent)
{
}

DataTableRow::DataTableRow(QObject* parent)
   : QObject(parent)
   , d_ptr(new DataTableRowPrivate(this))
{
}

void DataTableRow::set(QString const& key, QString const& value)
{
   Q_D(DataTableRow);
   d->columns.insert(key, value);
}

QStringList DataTableRow::keys() const
{
   Q_D(const DataTableRow);
   return d->columns.keys();
}

QString DataTableRow::value(QString const& key) const
{
   Q_D(const DataTableRow);
   Q_ASSERT(d->columns.contains(key));
   return d->columns.value(key);
}
