#pragma once

#include "currency.h"
#include "date.h"
#include "interval.h"
#include "ledgeritem.h"

class ItemProcessor;

class LedgerBudgetEntry : public LedgerItem
{
public:
   LedgerBudgetEntry(std::string const& filename, size_t lineNum);

   std::string category() const;
   void setCategory(std::string const& category);

   std::string owner() const;
   void setOwner(std::string const& owner);

private:
   std::string m_category;
   std::string m_owner;
};
