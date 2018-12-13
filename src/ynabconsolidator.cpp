#include "ynabconsolidator.h"

#include "ledgeritem.h"

YnabConsolidator::YnabConsolidator(QObject* parent) :
   QObject(parent),
   m_stopBudget(false),
   m_stopRegister(false)
{
}

void YnabConsolidator::processItem(QDate const& date, LedgerItem* item)
{
   m_items.insertMulti(date, item);
}

void YnabConsolidator::stopBudget()
{
   m_stopBudget = true;
   if (m_stopRegister)
   {
      foreach (LedgerItem* item_, m_items)
      {
         emit item(item_);
      }
      emit finished();
   }
}

void YnabConsolidator::stopRegister()
{
   m_stopRegister = true;
   if (m_stopBudget)
   {
      foreach (LedgerItem* item_, m_items)
      {
         emit item(item_);
      }
      emit finished();
   }
}
