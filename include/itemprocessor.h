#ifndef ITEMPROCESSOR_H
#define ITEMPROCESSOR_H

#include <QObject>

class LedgerAccountCommand;
class LedgerBudgetAllocation;
class LedgerComment;
class LedgerTransaction;

class ItemProcessor : public QObject
{
public:
   ItemProcessor(QObject* parent);
   virtual void processItem(LedgerAccountCommand const& account) = 0;
   virtual void processItem(LedgerBudgetAllocation const& allocation) = 0;
   virtual void processItem(LedgerComment const& comment) = 0;
   virtual void processItem(LedgerTransaction const& transaction) = 0;
   virtual void stop() = 0;
};

#endif
