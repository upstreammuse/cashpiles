#ifndef LEDGERITEM_H
#define LEDGERITEM_H

#include <QString>
class ItemProcessor;

class LedgerItem
{
public:
   LedgerItem(QString const& filename, int lineNum);
   virtual void processItem(ItemProcessor* processor) = 0;

private:
   QString m_filename;
   int m_lineNum;
};

#endif
