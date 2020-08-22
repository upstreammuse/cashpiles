#include "modelbuilder.h"

#include <cassert>
#include "budgetperiod.h"
#include "budgetuninitialized.h"
#include "../ledgerbudget.h"
#include "model.h"
#include "rubbish.h"

using namespace model;

using std::make_shared;

ModelBuilder::ModelBuilder(Model& model) :
   m_model{model}
{
}

#include <iostream>
bool ModelBuilder::processItem(LedgerBudget const& budget)
{
   std::cout << "model has a budget period date "
             << budget.date().toString("M/d/yyyy") << std::endl;
   try
   {
      auto currentBudget = m_model.budget.getCurrentBudget();
      if (budget.date() < currentBudget->period.startDate())
      {
         throw Rubbish("Cannot configure previous budget period");
      }
      currentBudget = m_model.budget.growBudgetPeriods(budget.date());
      if (budget.date() != currentBudget->period.startDate())
      {
         throw Rubbish("Budget configuration must happen on start date");
      }
   }
   catch (BudgetUninitialized)
   {
      auto currentBudget = m_model.budget.initializeBudget(
                              budget.date(), budget.interval(),
                              budget.lineNum());
   }
   auto currentBudget = m_model.budget.getCurrentBudget();
   assert(budget.date() == currentBudget->period.startDate());

   std::cout << "TODO configuring budget can take place" << std::endl;
   return false;
}
