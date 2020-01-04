#include "ledgeritem.h"

LedgerItem::LedgerItem(QString const& fileName, uint lineNum) :
   m_fileName(fileName),
   m_lineNum(lineNum)
{
}

LedgerItem::~LedgerItem()
{
}

QString LedgerItem::fileName() const
{
   return m_fileName;
}

uint LedgerItem::lineNum() const
{
   return m_lineNum;
}
