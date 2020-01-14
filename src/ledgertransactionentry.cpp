#include "ledgertransactionentry.h"

Currency LedgerTransactionEntry::amount() const
{
   return m_amount;
}

void LedgerTransactionEntry::setAmount(Currency const& amount)
{
   m_amount = amount;
}

Identifier LedgerTransactionEntry::category() const
{
   return m_category;
}

void LedgerTransactionEntry::setCategory(Identifier const& category)
{
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

Identifier LedgerTransactionEntry::payee() const
{
   return m_payee;
}

void LedgerTransactionEntry::setPayee(Identifier const& payee)
{
   m_payee = payee;
}
