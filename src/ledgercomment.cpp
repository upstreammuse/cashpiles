#include "ledgercomment.h"

LedgerComment::LedgerComment(int seqNum, int lineNum) :
   LedgerCommand(seqNum, lineNum)
{
}

QString LedgerComment::note() const
{
   return m_note;
}

void LedgerComment::setNote(QString const& note)
{
   m_note = note;
}
