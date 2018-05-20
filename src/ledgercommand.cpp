#include "ledgercommand.h"

LedgerCommand::LedgerCommand(int seqNum, int lineNum) :
   m_lineNum(lineNum),
   m_seqNum(seqNum)
{
}

int LedgerCommand::lineNum() const
{
   return m_lineNum;
}

int LedgerCommand::seqNum() const
{
   return m_seqNum;
}
