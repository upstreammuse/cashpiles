#pragma once

#include <memory>
#include <vector>

class ItemProcessor;
class LedgerItem;

class Ledger
{
public:
   void appendItem(std::shared_ptr<LedgerItem> item);
   void processItems(ItemProcessor& processor) const;

protected:
   std::vector<std::shared_ptr<LedgerItem>> m_items;
};
