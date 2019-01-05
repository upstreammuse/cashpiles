#include "ledgeritem.h"

int LedgerItem::m_nextId = 0;

LedgerItem::LedgerItem() :
   m_id(m_nextId++)
{
}

LedgerItem::LedgerItem(QString const& fileName, int lineNum) :
   LedgerItem()
{
   m_fileName = fileName;
   m_lineNum = lineNum;
}

LedgerItem::~LedgerItem()
{
}

QString LedgerItem::fileName() const
{
   return m_fileName;
}

int LedgerItem::id() const
{
   return m_id;
}

int LedgerItem::lineNum() const
{
   return m_lineNum;
}
