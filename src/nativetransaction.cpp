#include "nativetransaction.h"

NativeTransaction::NativeTransaction(int seqNum, int lineNum) :
   NativeCommand(seqNum, lineNum)
{
}

QString NativeTransaction::account() const
{
   return m_account;
}

void NativeTransaction::setAccount(QString const& account)
{
   m_account = account;
}

QString NativeTransaction::amount() const
{
   return m_amount;
}

void NativeTransaction::setAmount(QString const& amount)
{
   m_amount = amount;
}

QString NativeTransaction::balance() const
{
   Q_ASSERT(m_hasBalance);
   return m_balance;
}

bool NativeTransaction::hasBalance() const
{
   return m_hasBalance;
}

void NativeTransaction::setBalance(QString const& balance)
{
   m_hasBalance = true;
   m_balance = balance;
}

QString NativeTransaction::cleared() const
{
   return m_cleared;
}

void NativeTransaction::setCleared(QString const& cleared)
{
   m_cleared = cleared;
}

QString NativeTransaction::date() const
{
   return m_date;
}

void NativeTransaction::setDate(QString const& date)
{
   m_date = date;
}

QString NativeTransaction::note() const
{
   Q_ASSERT(m_hasNote);
   return m_note;
}

bool NativeTransaction::hasNote() const
{
   return m_hasNote;
}

void NativeTransaction::setNote(QString const& note)
{
   m_hasNote = true;
   m_note = note;
}

QList<NativeTransactionEntry> const& NativeTransaction::entries() const
{
   return m_entries;
}

void NativeTransaction::appendEntry(NativeTransactionEntry const& entry)
{
   m_entries.append(entry);
}
