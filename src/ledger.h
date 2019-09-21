#ifndef LEDGER_H
#define LEDGER_H

#include <QList>

class ItemProcessor;
class LedgerItem;

class Ledger
{
public:
   void processItems(ItemProcessor& processor);

public slots:
   void appendItem(QSharedPointer<LedgerItem> item);

protected:
   QList<QSharedPointer<LedgerItem>> m_items;
};

#endif
