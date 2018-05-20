#ifndef LEDGERCOMMENT_H
#define LEDGERCOMMENT_H

#include <QString>
#include "ledgercommand.h"

class LedgerComment : public LedgerCommand
{
public:
   LedgerComment(int seqNum, int lineNum);
   QString note() const;
   void setNote(QString const& note);

private:
   QString m_note;
};

#endif
