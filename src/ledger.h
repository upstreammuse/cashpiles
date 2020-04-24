#ifndef LEDGER_H
#define LEDGER_H

#include <memory>
#include <vector>

class ItemProcessor;
class LedgerItem;

class Ledger
{
public:
   void appendAfterCurrent(std::shared_ptr<LedgerItem> item);
   void appendItem(std::shared_ptr<LedgerItem> item);
   void clear();
   void processItems(ItemProcessor& processor);

private:
   std::vector<std::shared_ptr<LedgerItem>>::iterator m_currentItem;
   std::vector<std::shared_ptr<LedgerItem>> m_items;
};

#endif
