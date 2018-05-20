#ifndef NATIVECOMMAND_H
#define NATIVECOMMAND_H

class NativeCommand
{
public:
   NativeCommand(int seqNum, int lineNum);
   int lineNum() const;
   int seqNum() const;

private:
   int const m_lineNum;
   int const m_seqNum;
};

#endif
