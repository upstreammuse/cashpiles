#ifndef TXNTABLE_H
#define TXNTABLE_H

#include "account.h"
#include "transferpool.h"

#include <QMap>
#include <QObject>

class DataTable;

class TxnTable : public QObject
{
public:
   TxnTable(DataTable* data, QObject* parent = 0);

private:
   QMap<QString, Account> accounts;
   TransferPool transferPool;
};

#endif
