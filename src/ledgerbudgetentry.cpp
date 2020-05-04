#include "ledgerbudgetentry.h"

#include <cassert>
#include "itemprocessor.h"

LedgerBudgetEntry::LedgerBudgetEntry(std::string const& filename,
                                     size_t lineNum) :
   LedgerItem(filename, lineNum)
{
}

std::string LedgerBudgetEntry::category() const
{
   return m_category;
}

void LedgerBudgetEntry::setCategory(std::string const& category)
{
   m_category = category;
}

std::string LedgerBudgetEntry::owner() const
{
   return m_owner;
}

void LedgerBudgetEntry::setOwner(std::string const& owner)
{
   m_owner = owner;
}
