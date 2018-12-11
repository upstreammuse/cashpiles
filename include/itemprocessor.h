#ifndef ITEMPROCESSOR_H
#define ITEMPROCESSOR_H

class LedgerAccountCommand;
class LedgerBudgetAllocation;
class LedgerComment;
class LedgerTransaction;

class ItemProcessor
{
public:
   ItemProcessor();
   virtual void processItem(const LedgerAccountCommand& account) = 0;
   virtual void processItem(const LedgerBudgetAllocation& allocation) = 0;
   virtual void processItem(const LedgerComment& comment) = 0;
   virtual void processItem(const LedgerTransaction& transaction) = 0;
};

#endif
