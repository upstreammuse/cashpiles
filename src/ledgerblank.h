#pragma once

#include "ledgeritem.h"

class LedgerBlank : public LedgerItem
{
public:
   LedgerBlank(std::string const& fileName, size_t lineNum);
   LedgerBlank(LedgerItem const&);
   void processItem(ItemProcessor& processor) const;
};
