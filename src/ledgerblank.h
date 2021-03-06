#pragma once

#include "ledgeritem.h"

class LedgerBlank : public LedgerItem
{
public:
   LedgerBlank(std::string const& fileName, size_t lineNum);
   void processItem(ItemProcessor& processor) const;
};
