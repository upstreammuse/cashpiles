#ifndef TABTABLEROW_H
#define TABTABLEROW_H

#include <QObject>

class TabTableRowPrivate;

class TabTableRow : public QObject
{
public:
   TabTableRow(QString const& line, QObject* parent = 0);
   QList<int> indices() const;
   QString value(int index) const;

private:
   Q_DECLARE_PRIVATE(TabTableRow)
   TabTableRowPrivate* d_ptr;
};

#endif
