#include "ledgercomment.h"

#include "itemprocessor.h"

LedgerComment::LedgerComment(QString const& filename, int lineNum) :
   LedgerItem(filename, lineNum)
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

void LedgerComment::processItem(ItemProcessor* processor)
{
   processor->processItem(*this);
}
