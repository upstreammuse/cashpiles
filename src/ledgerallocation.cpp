#include "ledgerallocation.h"

#include "itemprocessor.h"

LedgerAllocation::LedgerAllocation(QString const& filename,
                                               int lineNum) :
   LedgerItem(filename, lineNum)
{
}

QMap<QString, Currency> LedgerAllocation::allocations() const
{
   return m_allocations;
}

void LedgerAllocation::appendAllocation(QString const& category,
                                        Currency const& amount)
{
   m_allocations[category] += amount;
}

QDate LedgerAllocation::date() const
{
   return m_date;
}

void LedgerAllocation::setDate(QDate const& date)
{
   m_date = date;
}

void LedgerAllocation::processItem(ItemProcessor* processor)
{
   processor->processItem(*this);
}
