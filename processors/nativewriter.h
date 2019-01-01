#ifndef NATIVEWRITER_H
#define NATIVEWRITER_H

#include "model/itemprocessor.h"

class NativeWriter : public ItemProcessor
{
public:
   NativeWriter(QObject* parent);
   void processItem(LedgerAccountCommand const& account);
   void processItem(LedgerBudget const& budget);
   void processItem(LedgerComment const& comment);
   void processItem(LedgerTransaction const& transaction);
   void stop();

private:
   void breakBetween();

private:
   bool m_firstItem = true;
};

#endif
