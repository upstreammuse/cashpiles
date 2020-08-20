#include "ledgerbudgetentry.h"

#include <cassert>
#include "itemprocessor.h"

using std::string;

LedgerBudgetEntry::LedgerBudgetEntry(std::string const& filename,
                                     size_t lineNum) :
   LedgerItem(filename, lineNum)
{
}

LedgerBudgetEntry::LedgerBudgetEntry(LedgerItem const& item) :
   LedgerItem(item)
{
}

string LedgerBudgetEntry::category() const
{
   return m_category;
}

void LedgerBudgetEntry::setCategory(string const& category)
{
   m_category = category;
}

string LedgerBudgetEntry::owner() const
{
   return m_owner;
}

void LedgerBudgetEntry::setOwner(string const& owner)
{
   m_owner = owner;
}
