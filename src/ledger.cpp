#include "ledger.h"

#include "itemprocessor.h"
#include "ledgeritem.h"

void Ledger::appendAfterCurrent(std::shared_ptr<LedgerItem> item)
{
   m_currentItem = m_items.insert(m_currentItem + 1, item) - 1;
}

// TODO need a way to ensure that this isn't called from a processor in tandem with appendAfterCurrent, since it will invalidate m_currentItem if it reallocates the vector
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
      auto ptr = *m_currentItem;
      ptr->processItem(processor);
   }
   processor.finish();
}
