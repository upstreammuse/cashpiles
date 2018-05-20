#ifndef LEDGERTRANSACTION_H
#define LEDGERTRANSACTION_H

#include <QDate>
#include <QList>
#include "ledgercommand.h"
#include "ledgertransactionentry.h"

class LedgerTransaction : public LedgerCommand
{
public:
   LedgerTransaction(int seqNum, int lineNum);

   QString account() const;
   void setAccount(QString const& account);

   void setBalance(int balance);

   void setCleared(bool cleared);

   QDate date() const;
   void setDate(QDate const& date);

   void setNote(QString const& note);

   QList<LedgerTransactionEntry> const& entries() const;
   void appendEntry(LedgerTransactionEntry const& entry);

private:
   QString m_account;
   int m_balance = 0;
   bool m_hasBalance = false;
   bool m_cleared = false;
   QDate m_date;
   QString m_note;
   bool m_hasNote = false;
   QList<LedgerTransactionEntry> m_entries;
};

#endif
