#ifndef LEDGER_H
#define LEDGER_H

#include <QMap>
#include <QObject>
#include "ledgertransaction.h"

class Ledger : public QObject
{
   Q_OBJECT

public:
   explicit Ledger(QObject* parent = 0);
   void addProcessor(ItemProcessor* processor);

public slots:
   void processItem(LedgerItem* item);
   void start();
   void stop();

signals:
   void finished();
   void started();

private:
   QList<ItemProcessor*> m_processors;
};

#endif
