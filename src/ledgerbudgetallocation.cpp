#include "ledgerbudgetallocation.h"

#include "itemprocessor.h"

LedgerBudgetAllocation::LedgerBudgetAllocation(QString const& filename,
                                               int lineNum) :
   LedgerItem(filename, lineNum)
{
}

QMap<QString, int> LedgerBudgetAllocation::allocations() const
{
   return m_allocations;
}

void LedgerBudgetAllocation::appendAllocation(QString const& category,
                                              int amount)
{
   if (!m_allocations.contains(category))
   {
      m_allocations[category] = 0;
   }
   m_allocations[category] += amount;
}

QDate LedgerBudgetAllocation::date() const
{
   return m_date;
}

void LedgerBudgetAllocation::setDate(QDate const& date)
{
   m_date = date;
}

void LedgerBudgetAllocation::processItem(ItemProcessor* processor)
{
   processor->processItem(*this);
}
