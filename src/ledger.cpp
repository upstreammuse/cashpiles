#include "ledger.h"

#include <QSharedPointer>
#include "itemprocessor.h"
#include "ledgeritem.h"

void Ledger::processItems(ItemProcessor& processor)
{
   processor.start();
   foreach (QSharedPointer<LedgerItem> item, m_items)
   {
      item->processItem(processor);
   }
   processor.finish();
}

void Ledger::appendItem(QSharedPointer<LedgerItem> item)
{
   m_items.append(item);
}
