#include "ledgerblank.h"

#include "itemprocessor.h"

LedgerBlank::LedgerBlank(QString const& fileName, uint lineNum) :
   LedgerItem(fileName, lineNum)
{

}

void LedgerBlank::processItem(ItemProcessor& processor) const
{
   processor.processItem(*this);
}
