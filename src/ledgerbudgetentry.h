#pragma once

#include "currency.h"
#include "date.h"
#include "identifier.h"
#include "interval.h"
#include "ledgeritem.h"

class ItemProcessor;

class LedgerBudgetEntry : public LedgerItem
{
public:
   LedgerBudgetEntry(std::string const& filename, size_t lineNum);

   Identifier category() const;
   void setCategory(Identifier const& category);

   Identifier owner() const;
   void setOwner(Identifier const& owner);

private:
   Identifier m_category;
   Identifier m_owner;
};
