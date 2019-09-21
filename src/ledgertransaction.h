#ifndef LEDGERTRANSACTION_H
#define LEDGERTRANSACTION_H

#include <QDate>
#include <QList>
#include "ledgertransactionentry.h"
#include "ledgeritem.h"

class LedgerTransaction : public LedgerItem
{
public:
   enum Status
   {
      CLEARED,
      PENDING,
      DISPUTED
   };
   static Status statusFromString(QString const& status, bool* ok);
   static QString statusToString(Status status);

public:
   LedgerTransaction(QString const& filename, uint lineNum);

   QString account() const;
   void setAccount(QString const& account);

   Currency amount() const;

   Currency balance() const;
   bool hasBalance() const;
   void setBalance(Currency const& balance);

   QDate date() const;
   void setDate(QDate const& date);

   QString note() const;
   bool hasNote() const;
   void setNote(QString const& note);

   Status status() const;
   void setStatus(Status status);

   // TODO it would be more symmetric to make this process the same way as the budget entries
   void appendEntry(LedgerTransactionEntry const& entry);
   QList<LedgerTransactionEntry> const& entries() const;

   void processItem(ItemProcessor& processor) const;

private:
   QString m_account;
   Currency m_balance;
   QDate m_date;
   QList<LedgerTransactionEntry> m_entries;
   bool m_hasBalance = false;
   bool m_hasNote = false;
   QString m_note;
   Status m_status = PENDING;
};

#endif
