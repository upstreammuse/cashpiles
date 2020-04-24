#include "ledgeritem.h"

int LedgerItem::sm_nextId = 1;

LedgerItem::LedgerItem() :
   id(sm_nextId++)
{
}

LedgerItem::LedgerItem(std::string const& fileName, size_t lineNum) :
   id(sm_nextId++),
   m_fileName(fileName),
   m_lineNum(lineNum)
{
   // TODO register the file/line with the "location manager" for tracking issues to locations in files
   //   once done, LI doesn't have to track file/line any more
   //   also consider putting the id registration in the file reader, so that LI doesn't have to care about file/line at all
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
