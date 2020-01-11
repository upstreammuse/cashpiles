#ifndef LEDGER_H
#define LEDGER_H

#include <QList>
#include <QSharedPointer>

class ItemProcessor;
class LedgerItem;

class Ledger
{
public:
   void processItems(ItemProcessor& processor);

public:
   void appendItem(QSharedPointer<LedgerItem> item);

protected:
   QList<QSharedPointer<LedgerItem>> m_items;
};

#endif
