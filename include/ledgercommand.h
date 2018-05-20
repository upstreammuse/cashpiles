#ifndef LEDGERCOMMAND_H
#define LEDGERCOMMAND_H

class LedgerCommand
{
public:
   LedgerCommand(int seqNum, int lineNum);
   int lineNum() const;
   int seqNum() const;

private:
   int const m_lineNum;
   int const m_seqNum;
};

#endif
