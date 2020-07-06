#include "budgetcancelentry.h"

#include "budgetgoalentry.h"

using namespace model;

BudgetCancelEntry::BudgetCancelEntry(
      std::shared_ptr<BudgetGoalEntry const> goal_) :
   BudgetEntry(goal_->budget),
   goal(goal_)
{
}
