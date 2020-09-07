#pragma once

class LedgerAccount;
class LedgerAccountBalance;
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
class LedgerTransactionV2;
class LedgerTransactionV2AccountEntry;
class LedgerTransactionV2CategoryEntry;
class LedgerTransactionV2OwnerEntry;

class ItemProcessor
{
public:
   virtual ~ItemProcessor();
   virtual void finish();
   virtual void finishBudget();
   virtual void processItem(LedgerAccount const& account);
   virtual void processItem(LedgerAccountBalance const& balance);
   virtual void processItem(LedgerBlank const& blank);
   virtual bool processItem(LedgerBudget const& budget);
   virtual void processItem(LedgerBudgetCancelEntry const& entry);
   virtual void processItem(LedgerBudgetCloseEntry const& entry);
   virtual void processItem(LedgerBudgetGoalEntry const& entry);
   virtual void processItem(LedgerBudgetGoalsEntry const& entry);
   virtual void processItem(LedgerBudgetIncomeEntry const& entry);
   virtual void processItem(LedgerBudgetReserveAmountEntry const& entry);
   virtual void processItem(LedgerBudgetReservePercentEntry const& entry);
   virtual void processItem(LedgerBudgetRoutineEntry const& entry);
   virtual void processItem(LedgerBudgetWithholdingEntry const& entry);
   virtual void processItem(LedgerComment const& comment);
   virtual void processItem(LedgerTransaction const& transaction);
   virtual bool processItem(LedgerTransactionV2 const& transaction);
   virtual void processItem(LedgerTransactionV2AccountEntry const& entry);
   virtual void processItem(LedgerTransactionV2CategoryEntry const& entry);
   virtual void processItem(LedgerTransactionV2OwnerEntry const& entry);
   virtual void start();
};
