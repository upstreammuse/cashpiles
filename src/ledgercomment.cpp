#include "ledgercomment.h"

#include "itemprocessor.h"

LedgerComment::LedgerComment(QString const& filename, uint lineNum) :
   LedgerItem(filename, lineNum)
{
}

QString LedgerComment::note() const
{
   return m_note;
}

void LedgerComment::processItem(ItemProcessor& processor) const
{
   processor.processItem(*this);
}

void LedgerComment::setNote(QString const& note)
{
   m_note = note;
}
