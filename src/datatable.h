#ifndef DATATABLE_H
#define DATATABLE_H

#include <QObject>

class DataTablePrivate;
class DataTableRow;

class DataTable : public QObject
{
public:
   DataTable(QObject* parent = 0);
   void addRow(DataTableRow* row);
   void setHeader(DataTableRow* header);
   void setTitle(QString const& title);

   DataTableRow* header() const;
   DataTableRow* row(int index) const;
   int rows() const;
   QString title() const;

private:
   Q_DECLARE_PRIVATE(DataTable)
   DataTablePrivate* d_ptr;
};

#endif
