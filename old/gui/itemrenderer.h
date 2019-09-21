#ifndef ITEMRENDERER_H
#define ITEMRENDERER_H

#include "kernel/itemprocessor.h"

class ItemRenderer : public ItemProcessor
{
   Q_OBJECT

public:
   ItemRenderer(QObject* parent);
   void processItem(LedgerAccountBalance const& balance);
   void processItem(LedgerAccountCommand const& account);
   void processItem(LedgerBudget const& budget);
   void processItem(LedgerComment const& comment);
   void processItem(LedgerTransaction const& transaction);
   void stop();

signals:
   void balance(LedgerAccountBalance const&) const;
   void transaction(LedgerTransaction const&) const;
};

#endif
