#ifndef TABTABLE_H
#define TABTABLE_H

#include <QObject>

class QTextStream;
class TabTablePrivate;
class TabTableRow;

class TabTable : public QObject
{
public:
   TabTable(QObject* parent = 0);
   void addRow(TabTableRow* row);
   void setHeader(TabTableRow* header);
   void setTitle(QString const& title);

   TabTableRow* header() const;
   TabTableRow* row(int index) const;
   int rows() const;
   QString title() const;

private:
   Q_DECLARE_PRIVATE(TabTable)
   TabTablePrivate* d_ptr;
};

QTextStream& operator>>(QTextStream& in, TabTable& tt);

#endif
