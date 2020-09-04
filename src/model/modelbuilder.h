#pragma once

#include "../itemprocessor.h"

namespace model { struct Model; }
namespace model { class ModelBuilder; }

class model::ModelBuilder : public ItemProcessor
{
public:
   ModelBuilder(Model&);
   bool processItem(LedgerBudget const&);
//   void processItem(LedgerBudgetCancelEntry const& entry);
//   void processItem(LedgerBudgetCloseEntry const& entry);
//   void processItem(LedgerBudgetGoalEntry const& entry);
//   void processItem(LedgerBudgetGoalsEntry const& entry);
//   void processItem(LedgerBudgetIncomeEntry const& entry);
//   void processItem(LedgerBudgetReserveAmountEntry const& entry);
//   void processItem(LedgerBudgetReservePercentEntry const& entry);
   void processItem(LedgerBudgetRoutineEntry const& entry);
//   void processItem(LedgerBudgetWithholdingEntry const& entry);

private:
   Model& m_model;
};
