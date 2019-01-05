#ifndef ITEMPROCESSOR_H
#define ITEMPROCESSOR_H

#include <QObject>

class LedgerAccountBalance;
class LedgerAccountCommand;
class LedgerBudget;
class LedgerComment;
class LedgerTransaction;

class ItemProcessor : public QObject
{
public:
   ItemProcessor(QObject* parent);
   virtual void processItem(LedgerAccountBalance const& balance) = 0;
   virtual void processItem(LedgerAccountCommand const& account) = 0;
   virtual void processItem(LedgerBudget const& budget) = 0;
   virtual void processItem(LedgerComment const& comment) = 0;
   virtual void processItem(LedgerTransaction const& transaction) = 0;
   virtual void stop() = 0;
};

#endif
