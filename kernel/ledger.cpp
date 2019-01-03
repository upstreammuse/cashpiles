#include "ledger.h"

#include "itemprocessor.h"

Ledger::Ledger(QObject *parent) :
   QObject(parent)
{
}

void Ledger::addProcessor(ItemProcessor* processor)
{
   processor->setParent(this);
   m_processors.append(processor);
}

void Ledger::processItem(LedgerItem* item)
{
   foreach (ItemProcessor* processor, m_processors)
   {
      item->processItem(processor);
   }
   delete item;
}

void Ledger::start()
{
   emit started();
}

void Ledger::stop()
{
   foreach (ItemProcessor* processor, m_processors)
   {
      processor->stop();
   }
   emit finished();
}
