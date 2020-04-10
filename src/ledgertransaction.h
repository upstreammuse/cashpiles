#ifndef LEDGERTRANSACTION_H
#define LEDGERTRANSACTION_H

#include <QDate>
#include <QList>
#include <QSharedPointer>
#include "identifier.h"
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
   static Status statusFromString(std::string const& status, bool* ok);
   static QString statusToString(Status status);

public:
   LedgerTransaction(QString const& filename, uint lineNum);

   Identifier account() const;
   void setAccount(Identifier const& account);

   Currency amount() const;

   Currency balance() const;
   bool hasBalance() const;
   void setBalance(Currency const& balance);

   QDate date() const;
   void setDate(QDate const& date);

   QString note() const;
   bool hasNote() const;
   void setNote(std::string const& note);

   Status status() const;
   void setStatus(Status status);

   void appendEntry(LedgerTransactionEntry const& entry);
   QList<LedgerTransactionEntry> const& entries() const;

   void processItem(ItemProcessor& processor) const;

private:
   Identifier m_account;
   QSharedPointer<Currency> m_balance;
   QDate m_date;
   QList<LedgerTransactionEntry> m_entries;
   QSharedPointer<QString> m_note;
   Status m_status = PENDING;
};

#endif
