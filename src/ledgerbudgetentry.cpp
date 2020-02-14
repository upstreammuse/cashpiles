#include "ledgerbudgetentry.h"

#include "itemprocessor.h"

LedgerBudgetEntry::LedgerBudgetEntry(QString const& filename, uint lineNum) :
   LedgerItem(filename, lineNum)
{
}

Identifier LedgerBudgetEntry::category() const
{
   return m_category;
}

void LedgerBudgetEntry::setCategory(Identifier const& category)
{
   Q_ASSERT(category.type() == Identifier::Type::CATEGORY);
   m_category = category;
}

QDate LedgerBudgetEntry::date() const
{
   return m_date;
}

void LedgerBudgetEntry::setDate(QDate const& date)
{
   Q_ASSERT(date.isValid());
   m_date = date;
}

Identifier LedgerBudgetEntry::owner() const
{
   return m_owner;
}

void LedgerBudgetEntry::setOwner(Identifier const& owner)
{
   Q_ASSERT(owner.type() == Identifier::Type::OWNER);
   m_owner = owner;
}
