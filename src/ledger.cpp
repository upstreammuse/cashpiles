#include "ledger.h"

#include <cassert>
#include "itemprocessor.h"
#include "ledgeritem.h"

void Ledger::appendItem(std::shared_ptr<LedgerItem> item)
{
   m_items.push_back(item);
}

void Ledger::clear()
{
   m_items.clear();
}

void Ledger::processItems(ItemProcessor& processor)
{
   processor.start();
   for (m_currentItem = m_items.begin(); m_currentItem != m_items.end();
        ++m_currentItem)
   {
      // copy the pointer out of the vector so we can replace it recursively
      // while this call is in progress
      std::shared_ptr<LedgerItem> ptr(*m_currentItem);
      ptr->processItem(processor);
   }
   processor.finish();
}

void Ledger::replaceItem(std::shared_ptr<LedgerItem> item)
{
   *m_currentItem = item;
}
