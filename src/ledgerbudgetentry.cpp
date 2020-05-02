#include "ledgerbudgetentry.h"

#include <cassert>
#include "itemprocessor.h"

LedgerBudgetEntry::LedgerBudgetEntry(std::string const& filename,
                                     size_t lineNum) :
   LedgerItem(filename, lineNum)
{
}

Identifier LedgerBudgetEntry::category() const
{
   return m_category;
}

void LedgerBudgetEntry::setCategory(Identifier const& category)
{
   assert(category.type() == Identifier::Type::CATEGORY);
   m_category = category;
}

Identifier LedgerBudgetEntry::owner() const
{
   return m_owner;
}

void LedgerBudgetEntry::setOwner(Identifier const& owner)
{
   assert(owner.type() == Identifier::Type::OWNER);
   m_owner = owner;
}
