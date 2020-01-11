#ifndef LEDGERBLANK_H
#define LEDGERBLANK_H

#include "ledgeritem.h"

class LedgerBlank : public LedgerItem
{
public:
   LedgerBlank(QString const& fileName, uint lineNum);
   void processItem(ItemProcessor& processor) const;
};

#endif
