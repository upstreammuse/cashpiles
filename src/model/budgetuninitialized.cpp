#include "budgetuninitialized.h"

using namespace model;

BudgetUninitialized::BudgetUninitialized() :
   logic_error("Budget is not initialized yet")
{
}
