#include "ledgerbudgetallocation.h"

#include "itemprocessor.h"

LedgerBudgetAllocation::LedgerBudgetAllocation(QString const& filename,
                                               int lineNum) :
   LedgerItem(filename, lineNum)
{
}

QMap<QString, Currency> LedgerBudgetAllocation::allocations() const
{
   return m_allocations;
}

void LedgerBudgetAllocation::appendAllocation(QString const& category,
                                              Currency const& amount)
{
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
