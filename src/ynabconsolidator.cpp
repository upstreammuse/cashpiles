#include "ynabconsolidator.h"

YnabConsolidator::YnabConsolidator(QObject* parent) :
   QObject(parent),
   m_stopBudget(false),
   m_stopRegister(false)
{
}

void YnabConsolidator::processItem(LedgerItem *item_)
{
   emit item(item_);
}

void YnabConsolidator::stopBudget()
{
   m_stopBudget = true;
   if (m_stopRegister)
   {
      emit finished();
   }
}

void YnabConsolidator::stopRegister()
{
   m_stopRegister = true;
   if (m_stopBudget)
   {
      emit finished();
   }
}
