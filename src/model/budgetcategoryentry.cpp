#include "budgetcategoryentry.h"

using namespace model;

BudgetCategoryEntry::BudgetCategoryEntry(
      std::string const& name_, std::shared_ptr<BudgetPeriod const> period,
      std::shared_ptr<BudgetCategoryOwner const> owner_) :
   BudgetEntry(period),
   name(name_),
   owner(owner_)
{
}
