#ifndef ITEMRENDERER_H
#define ITEMRENDERER_H

#include "kernel/itemprocessor.h"

class ItemRenderer : public ItemProcessor
{
   Q_OBJECT

public:
   ItemRenderer(QObject* parent);
   void processItem(LedgerAccountCommand const& account);
   void processItem(LedgerBudget const& budget);
   void processItem(LedgerComment const& comment);
   void processItem(LedgerTransaction const& transaction);
   void stop();

signals:
   void transaction(LedgerTransaction const&) const;
};

#endif
