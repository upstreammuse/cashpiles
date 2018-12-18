#include "ledgertransactionentry.h"

Currency LedgerTransactionEntry::amount() const
{
   return m_amount;
}

void LedgerTransactionEntry::setAmount(Currency const& amount)
{
   m_amount = amount;
}

QString LedgerTransactionEntry::category() const
{
   Q_ASSERT(m_hasCategory);
   return m_category;
}

bool LedgerTransactionEntry::hasCategory() const
{
   return m_hasCategory;
}

void LedgerTransactionEntry::setCategory(QString const& category)
{
   m_hasCategory = true;
   m_category = category;
}

QString LedgerTransactionEntry::note() const
{
   Q_ASSERT(m_hasNote);
   return m_note;
}

bool LedgerTransactionEntry::hasNote() const
{
   return m_hasNote;
}

void LedgerTransactionEntry::setNote(QString const& note)
{
   m_hasNote = true;
   m_note = note;
}

QString LedgerTransactionEntry::payee() const
{
   return m_payee;
}

void LedgerTransactionEntry::setPayee(QString const& payee)
{
   m_payee = payee;
}

bool LedgerTransactionEntry::transfer() const
{
   return m_transfer;
}

void LedgerTransactionEntry::setTransfer(bool transfer)
{
   m_transfer = transfer;
}
