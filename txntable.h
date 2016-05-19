#ifndef TXNTABLE_H
#define TXNTABLE_H

#include "account.h"

#include <QMap>
#include <QObject>

class DataTable;

class TxnTable : public QObject
{
public:
   TxnTable(DataTable* data, QObject* parent = 0);
   QString account() const;
   QList<Txn> txns() const;

private:
   // TODO QPriv
   QString m_account;
   QList<Txn> m_txns;
};

#endif
