#ifndef LEDGERCOMMENT_H
#define LEDGERCOMMENT_H

#include "ledgeritem.h"

class LedgerComment : public LedgerItem
{
public:
   LedgerComment(QString const& filename, int lineNum);

   QString note() const;
   void setNote(QString const& note);

   void processItem(ItemProcessor* processor);

private:
   QString m_note;
};

#endif
