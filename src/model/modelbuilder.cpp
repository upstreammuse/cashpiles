#include "modelbuilder.h"

#include <cassert>
#include "budgetperiod.h"
#include "budgetuninitialized.h"
#include "../ledgerbudget.h"
#include "../ledgerbudgetroutineentry.h"
#include "model.h"
#include "rubbish.h"

using namespace model;

using std::make_shared;

ModelBuilder::ModelBuilder(Model& model) :
   m_model{model}
{
}

bool ModelBuilder::processItem(LedgerBudget const& budget)
{
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
      m_model.budget.initializeBudget(
               budget.date(), budget.interval(), budget.lineNum());
   }

   auto currentBudget = m_model.budget.getCurrentBudget();
   assert(budget.date() == currentBudget->period.startDate());
   m_model.budget.configureBudget(currentBudget->id, budget.interval());
   return true;
}

void ModelBuilder::processItem(LedgerBudgetRoutineEntry const& entry)
{
   auto currentBudget = m_model.budget.getCurrentBudget();
   m_model.budget.createRoutineEntry(
            currentBudget->id, entry.category(), entry.owner(),
            entry.lineNum());
}
