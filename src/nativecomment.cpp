#include "nativecomment.h"

NativeComment::NativeComment(int seqNum, int lineNum) :
   NativeCommand(seqNum, lineNum)
{
}

QString NativeComment::note() const
{
   return m_note;
}

void NativeComment::setNote(QString const& note)
{
   m_note = note;
}
