#ifndef DATATABLEFACTORY_H
#define DATATABLEFACTORY_H

#include <QObject>

class DataTable;
class TabTable;

class DataTableFactory : public QObject
{
public:
   DataTable* create(TabTable* tt);
};

#endif
