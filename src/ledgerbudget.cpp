#include "ledgerbudget.h"

#include <QSharedPointer>
#include "itemprocessor.h"

LedgerBudget::LedgerBudget(QString const& fileName, uint lineNum) :
   LedgerItem(fileName, lineNum)
{
}

void LedgerBudget::appendEntry(QSharedPointer<LedgerBudgetEntry> entry)
{
   entry->setDate(m_date);
   m_entries.append(entry);
}

QDate LedgerBudget::date() const
{
   return m_date;
}

Interval LedgerBudget::interval() const
{
   return m_interval;
}

void LedgerBudget::processItem(ItemProcessor& processor) const
{
   processor.processItem(*this);
   foreach (QSharedPointer<LedgerBudgetEntry> entry, m_entries)
   {
      entry->processItem(processor);
   }
}

void LedgerBudget::setDate(QDate const& date)
{
   m_date = date;
   foreach (QSharedPointer<LedgerBudgetEntry> entry, m_entries)
   {
      entry->setDate(m_date);
   }
}

void LedgerBudget::setInterval(Interval interval)
{
   m_interval = interval;
}
