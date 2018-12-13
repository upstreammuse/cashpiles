#include "ledgertransaction.h"

#include "itemprocessor.h"

LedgerTransaction::LedgerTransaction(QString const& filename, int lineNum) :
   LedgerItem(filename, lineNum)
{
}

QString LedgerTransaction::account() const
{
   return m_account;
}

void LedgerTransaction::setAccount(QString const& account)
{
   m_account = account;
}

int LedgerTransaction::amount() const
{
   int amount = 0;
   foreach (LedgerTransactionEntry const& entry, m_entries)
   {
      amount += entry.amount();
   }
   return amount;
}

int LedgerTransaction::balance() const
{
   Q_ASSERT(m_hasBalance);
   return m_balance;
}

bool LedgerTransaction::hasBalance() const
{
   return m_hasBalance;
}

void LedgerTransaction::setBalance(int balance)
{
   m_hasBalance = true;
   m_balance = balance;
}

bool LedgerTransaction::cleared() const
{
   return m_cleared;
}

void LedgerTransaction::setCleared(bool cleared)
{
   m_cleared = cleared;
}

QDate LedgerTransaction::date() const
{
   return m_date;
}

void LedgerTransaction::setDate(QDate const& date)
{
   m_date = date;
}

QString LedgerTransaction::note() const
{
   Q_ASSERT(m_hasNote);
   return m_note;
}

bool LedgerTransaction::hasNote() const
{
   return m_hasNote;
}

void LedgerTransaction::setNote(QString const& note)
{
   m_hasNote = true;
   m_note = note;
}

QList<LedgerTransactionEntry> const& LedgerTransaction::entries() const
{
   return m_entries;
}

void LedgerTransaction::appendEntry(LedgerTransactionEntry const& entry)
{
   m_entries.append(entry);
}

void LedgerTransaction::processItem(ItemProcessor* processor)
{
   processor->processItem(*this);
}
