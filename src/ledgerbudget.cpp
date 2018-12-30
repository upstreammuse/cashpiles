#include "ledgerbudget.h"

#include "itemprocessor.h"

LedgerBudget::LedgerBudget(QString const& fileName, int lineNum) :
   LedgerItem(fileName, lineNum)
{
}

QHash<QString, LedgerBudget::Category> LedgerBudget::categories() const
{
   return m_categories;
}

void LedgerBudget::insertCategory(QString const& name, Category const& category)
{
   m_categories[name] = category;
}

QDate LedgerBudget::date() const
{
   return m_date;
}

void LedgerBudget::setDate(QDate const& date)
{
   m_date = date;
}

Interval LedgerBudget::interval() const
{
   return m_interval;
}

void LedgerBudget::setInterval(Interval interval)
{
   m_interval = interval;
}

void LedgerBudget::processItem(ItemProcessor* processor)
{
   processor->processItem(*this);
}
