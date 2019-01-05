#ifndef LEDGERTRANSACTION_H
#define LEDGERTRANSACTION_H

#include <QDate>
#include <QList>
#include "ledgertransactionentry.h"
#include "ledgeritem.h"

class LedgerTransaction : public LedgerItem
{
public:
   LedgerTransaction(QString const& filename, int lineNum);

   QString account() const;
   void setAccount(QString const& account);

   Currency amount() const;

   bool cleared() const;
   void setCleared(bool cleared);

   QDate date() const;
   void setDate(QDate const& date);

   QString note() const;
   bool hasNote() const;
   void setNote(QString const& note);

   QList<LedgerTransactionEntry> const& entries() const;
   void appendEntry(LedgerTransactionEntry const& entry);

   void processItem(ItemProcessor* processor);

private:
   QString m_account;
   Currency m_balance;
   bool m_hasBalance = false;
   bool m_cleared = false;
   QDate m_date;
   QString m_note;
   bool m_hasNote = false;
   QList<LedgerTransactionEntry> m_entries;
};

#endif
