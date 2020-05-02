#ifndef ITEMPROCESSOR_H
#define ITEMPROCESSOR_H

class LedgerAccount;
class LedgerBlank;
class LedgerBudget;
class LedgerBudgetCancelEntry;
class LedgerBudgetCloseEntry;
class LedgerBudgetGoalEntry;
class LedgerBudgetGoalsEntry;
class LedgerBudgetIncomeEntry;
class LedgerBudgetReserveAmountEntry;
class LedgerBudgetReservePercentEntry;
class LedgerBudgetRoutineEntry;
class LedgerBudgetWithholdingEntry;
class LedgerComment;
class LedgerTransaction;

class ItemProcessor
{
public:
   virtual ~ItemProcessor();
   virtual void finish();
   virtual void processItem(LedgerAccount const& account);
   virtual void processItem(LedgerBlank const& blank);
   virtual void processItem(LedgerBudget const& budget);
   virtual void processItem(LedgerBudgetCancelEntry const& budget);
   virtual void processItem(LedgerBudgetCloseEntry const& budget);
   virtual void processItem(LedgerBudgetGoalEntry const& budget);
   virtual void processItem(LedgerBudgetGoalsEntry const& budget);
   virtual void processItem(LedgerBudgetIncomeEntry const& budget);
   virtual void processItem(LedgerBudgetReserveAmountEntry const& budget);
   virtual void processItem(LedgerBudgetReservePercentEntry const& budget);
   virtual void processItem(LedgerBudgetRoutineEntry const& budget);
   virtual void processItem(LedgerBudgetWithholdingEntry const& budget);
   virtual void processItem(LedgerComment const& comment);
   virtual void processItem(LedgerTransaction const& transaction);
   virtual void start();
};

#endif
