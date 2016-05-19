#ifndef DATATABLEROW_H
#define DATATABLEROW_H

#include <QObject>

class DataTableRowPrivate;

class DataTableRow : public QObject
{
public:
   DataTableRow(QObject* parent = 0);
   void set(QString const& key, QString const& value);

   QStringList keys() const;
   QString value(QString const& key) const;

private:
   Q_DECLARE_PRIVATE(DataTableRow)
   DataTableRowPrivate* d_ptr;
};

#endif
