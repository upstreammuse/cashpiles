#ifndef LEDGERITEM_H
#define LEDGERITEM_H

#include <QString>

class ItemProcessor;

class LedgerItem
{
public:
   LedgerItem();
   LedgerItem(QString const& fileName, uint lineNum);
   virtual ~LedgerItem();
   QString fileName() const;
   uint lineNum() const;
   virtual void processItem(ItemProcessor& processor) const = 0;

private:
   QString m_fileName;
   uint m_lineNum = 0;
};

#endif
