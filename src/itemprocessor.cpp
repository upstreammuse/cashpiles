#include "itemprocessor.h"

ItemProcessor::~ItemProcessor()
{
}

void ItemProcessor::finish()
{
}

void ItemProcessor::processItem(LedgerAccount const&)
{
}

void ItemProcessor::processItem(LedgerBlank const&)
{
}

bool ItemProcessor::processItem(LedgerBudget const&)
{
   return true;
}

void ItemProcessor::processItem(LedgerBudgetCancelEntry const&)
{
}

void ItemProcessor::processItem(LedgerBudgetCloseEntry const&)
{
}

void ItemProcessor::processItem(LedgerBudgetGoalEntry const&)
{
}

void ItemProcessor::processItem(LedgerBudgetGoalsEntry const&)
{
}

void ItemProcessor::processItem(LedgerBudgetIncomeEntry const&)
{
}

void ItemProcessor::processItem(LedgerBudgetReserveAmountEntry const&)
{
}

void ItemProcessor::processItem(LedgerBudgetReservePercentEntry const&)
{
}

void ItemProcessor::processItem(LedgerBudgetRoutineEntry const&)
{
}

void ItemProcessor::processItem(LedgerBudgetWithholdingEntry const&)
{
}

void ItemProcessor::processItem(LedgerComment const&)
{
}

void ItemProcessor::processItem(LedgerTransaction const&)
{
}

bool ItemProcessor::processItem(LedgerTransactionV2 const&)
{
   return true;
}

void ItemProcessor::processItem(LedgerTransactionV2AccountEntry const&)
{
}

void ItemProcessor::processItem(LedgerTransactionV2CategoryEntry const&)
{
}

void ItemProcessor::processItem(LedgerTransactionV2OwnerEntry const&)
{
}

void ItemProcessor::start()
{
}
