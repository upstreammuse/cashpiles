#include "ledgerbudget.h"

#include "itemprocessor.h"

LedgerBudget::LedgerBudget(QString const& fileName, int lineNum) :
   LedgerItem(fileName, lineNum)
{
}

QSet<QString> LedgerBudget::categories() const
{
   return m_categories;
}

void LedgerBudget::addCategory(QString const& category)
{
   m_categories.insert(category);
}

QDate LedgerBudget::date() const
{
   return m_date;
}

void LedgerBudget::setDate(QDate const& date)
{
   m_date = date;
}

void LedgerBudget::processItem(ItemProcessor* processor)
{
   processor->processItem(*this);
}
