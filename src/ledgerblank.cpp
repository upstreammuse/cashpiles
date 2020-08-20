#include "ledgerblank.h"

#include "itemprocessor.h"

LedgerBlank::LedgerBlank(std::string const& fileName, size_t lineNum) :
   LedgerItem(fileName, lineNum)
{
}

LedgerBlank::LedgerBlank(LedgerItem const& item) :
   LedgerItem(item)
{
}

void LedgerBlank::processItem(ItemProcessor& processor) const
{
   processor.processItem(*this);
}
