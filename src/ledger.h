#ifndef LEDGER_H
#define LEDGER_H

#include <memory>
#include <QList>
#include <QSharedPointer>

class ItemProcessor;
class LedgerItem;

class Ledger
{
public:
   void processItems(ItemProcessor& processor);

public:
   void appendItem(std::shared_ptr<LedgerItem> item);

protected:
   std::vector<std::shared_ptr<LedgerItem>> m_items;
};

#endif
