#ifndef LEDGERITEM_H
#define LEDGERITEM_H

#include <QString>
class ItemProcessor;

class LedgerItem
{
public:
   LedgerItem();
   LedgerItem(QString const& fileName, int lineNum);
   virtual ~LedgerItem();
   QString fileName() const;
   int id() const;
   int lineNum() const;
   virtual void processItem(ItemProcessor* processor) = 0;

private:
   static int m_nextId;

private:
   QString m_fileName;
   int m_id;
   int m_lineNum = 0;
};

#endif
