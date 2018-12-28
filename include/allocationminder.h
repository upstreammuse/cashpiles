#ifndef ALLOCATIONMINDER_H
#define ALLOCATIONMINDER_H

#include "itemprocessor.h"

class AllocationMinder : public ItemProcessor
{
public:
   AllocationMinder(QObject* parent);
   void processItem(LedgerAccountCommand const& account);
   void processItem(LedgerAllocation const& allocation);
   void processItem(LedgerBudget const& budget);
   void processItem(LedgerComment const& comment);
   void processItem(LedgerTransaction const& transaction);
   void stop();
};

#endif
