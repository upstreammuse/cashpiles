#ifndef ITEMPROCESSOR_H
#define ITEMPROCESSOR_H

class LedgerAccount;
class LedgerBudget;
class LedgerBudgetGoalEntry;
class LedgerBudgetIncomeEntry;
class LedgerBudgetReserveAmountEntry;
class LedgerBudgetReservePercentEntry;
class LedgerBudgetRoutineEntry;
class LedgerComment;
class LedgerTransaction;

class ItemProcessor
{
public:
   virtual ~ItemProcessor();
   virtual void finish() = 0;
   virtual void processItem(LedgerAccount const& account) = 0;
   virtual void processItem(LedgerBudget const& budget) = 0;
   virtual void processItem(LedgerBudgetGoalEntry const& budget) = 0;
   virtual void processItem(LedgerBudgetIncomeEntry const& budget) = 0;
   virtual void processItem(LedgerBudgetReserveAmountEntry const& budget) = 0;
   virtual void processItem(LedgerBudgetReservePercentEntry const& budget) = 0;
   virtual void processItem(LedgerBudgetRoutineEntry const& budget) = 0;
   virtual void processItem(LedgerComment const& comment) = 0;
   virtual void processItem(LedgerTransaction const& transaction) = 0;
   virtual void start() = 0;
};

#endif
