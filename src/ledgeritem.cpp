#include "ledgeritem.h"

LedgerItem::LedgerItem(QString const& fileName, int lineNum) :
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

int LedgerItem::lineNum() const
{
   return m_lineNum;
}
