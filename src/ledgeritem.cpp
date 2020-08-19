#include "ledgeritem.h"

LedgerItem::LedgerItem(std::string const& fileName, size_t lineNum) :
   m_fileName(fileName),
   m_lineNum(lineNum)
{
}

LedgerItem::~LedgerItem()
{
}

std::string LedgerItem::fileName() const
{
   return m_fileName;
}

size_t LedgerItem::lineNum() const
{
   return m_lineNum;
}

std::pair<std::string, bool> LedgerItem::note() const
{
   return m_note;
}

void LedgerItem::setNote(std::string const& note)
{
   m_note = make_pair(note, true);
}
