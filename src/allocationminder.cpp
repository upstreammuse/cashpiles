#include "allocationminder.h"

AllocationMinder::AllocationMinder(QObject* parent) :
   ItemProcessor(parent)
{
}

void AllocationMinder::processItem(LedgerAccountCommand const&)
{
}

void AllocationMinder::processItem(LedgerAllocation const& allocation)
{
}

void AllocationMinder::processItem(LedgerBudget const& budget)
{
}

void AllocationMinder::processItem(LedgerComment const&)
{
}

void AllocationMinder::processItem(LedgerTransaction const& transaction)
{
}

void AllocationMinder::stop()
{
}
