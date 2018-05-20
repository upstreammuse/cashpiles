#ifndef NATIVECOMMENT_H
#define NATIVECOMMENT_H

#include <QString>

#include "nativecommand.h"

class NativeComment : public NativeCommand
{
public:
   NativeComment(int seqNum, int lineNum);
   QString note() const;
   void setNote(QString const& note);

private:
   QString m_note;
};

#endif
