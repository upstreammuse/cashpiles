#include "ledger.h"

#include "itemprocessor.h"
#include "ledgeritem.h"

void Ledger::clear()
{
   m_items.clear();
}

void Ledger::processItems(ItemProcessor& processor)
{
   processor.start();
   for (auto it : m_items)
   {
      it->processItem(processor);
   }
   processor.finish();
}

void Ledger::appendItem(std::shared_ptr<LedgerItem> item)
{
   m_items.push_back(item);
}
