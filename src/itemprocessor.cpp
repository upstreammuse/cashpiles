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

void ItemProcessor::processItem(LedgerBudget const&)
{
}

void ItemProcessor::processItem(LedgerBudgetGoalEntry const&)
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

void ItemProcessor::processItem(LedgerComment const&)
{
}

void ItemProcessor::processItem(LedgerReserve const&)
{
}

void ItemProcessor::processItem(LedgerTransaction const&)
{
}

void ItemProcessor::start()
{
}