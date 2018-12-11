#include "ledgeritem.h"

LedgerItem::LedgerItem(QString const& filename, int lineNum) :
   m_filename(filename),
   m_lineNum(lineNum)
{
}
