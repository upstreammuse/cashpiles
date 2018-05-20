#ifndef NATIVETRANSACTION_H
#define NATIVETRANSACTION_H

#include <QList>
#include <QString>
#include "nativecommand.h"
#include "nativetransactionentry.h"

class NativeTransaction : public NativeCommand
{
public:
   NativeTransaction(int seqNum, int lineNum);

   QString account() const;
   void setAccount(QString const& account);

   QString amount() const;
   void setAmount(QString const& amount);

   QString balance() const;
   bool hasBalance() const;
   void setBalance(QString const& balance);

   QString cleared() const;
   void setCleared(QString const& cleared);

   QString date() const;
   void setDate(QString const& date);

   QString note() const;
   bool hasNote() const;
   void setNote(QString const& note);

   QList<NativeTransactionEntry> const& entries() const;
   void appendEntry(NativeTransactionEntry const& entry);

private:
   QString m_account;
   QString m_amount;
   QString m_balance;
   bool m_hasBalance = false;
   QString m_cleared;
   QString m_date;
   QString m_note;
   bool m_hasNote = false;
   QList<NativeTransactionEntry> m_entries;
};

#endif
