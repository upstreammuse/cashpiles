#ifndef LEDGERITEM_H
#define LEDGERITEM_H

#include <QString>
class ItemProcessor;

class LedgerItem
{
public:
   LedgerItem(QString const& fileName, int lineNum);
   virtual ~LedgerItem();
   QString fileName() const;
   int lineNum() const;
   virtual void processItem(ItemProcessor* processor) = 0;

private:
   QString m_fileName;
   int m_lineNum;
};

#endif
