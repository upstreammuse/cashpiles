#ifndef LEDGERCOMMENT_H
#define LEDGERCOMMENT_H

#include "ledgeritem.h"

class LedgerComment : public LedgerItem
{
public:
   LedgerComment(QString const& filename, uint lineNum);
   QString note() const;
   void processItem(ItemProcessor& processor) const;
   void setNote(QString const& note);

private:
   QString m_note;
};

#endif
