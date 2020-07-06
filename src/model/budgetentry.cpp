#include "budgetentry.h"

using namespace model;

BudgetEntry::BudgetEntry(std::shared_ptr<BudgetPeriod const> budget_) :
   budget(budget_)
{
}
