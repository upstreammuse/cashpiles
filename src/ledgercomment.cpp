#include "ledgercomment.h"

#include "itemprocessor.h"

LedgerComment::LedgerComment(std::string const& filename, size_t lineNum) :
   LedgerItem(filename, lineNum)
{
}

std::string LedgerComment::note() const
{
   return m_note;
}

void LedgerComment::processItem(ItemProcessor& processor) const
{
   processor.processItem(*this);
}

void LedgerComment::setNote(std::string const& note)
{
   m_note = note;
}
