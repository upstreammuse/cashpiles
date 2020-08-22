#include "ledger.h"

#include "itemprocessor.h"
#include "ledgeritem.h"

void Ledger::appendItem(std::shared_ptr<LedgerItem> item)
{
   m_items.push_back(item);
}

void Ledger::processItems(ItemProcessor& processor) const
{
   processor.start();
   for (auto it : m_items)
   {
      it->processItem(processor);
   }
   processor.finish();
}
