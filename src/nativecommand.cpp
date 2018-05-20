#include "nativecommand.h"

NativeCommand::NativeCommand(int seqNum, int lineNum) :
   m_lineNum(lineNum),
   m_seqNum(seqNum)
{
}

int NativeCommand::lineNum() const
{
   return m_lineNum;
}

int NativeCommand::seqNum() const
{
   return m_seqNum;
}
