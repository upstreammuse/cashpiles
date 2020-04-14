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
