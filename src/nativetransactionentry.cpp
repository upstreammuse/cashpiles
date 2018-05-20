#include "nativetransactionentry.h"

QString NativeTransactionEntry::amount() const
{
   return m_amount;
}

void NativeTransactionEntry::setAmount(QString const& amount)
{
   m_amount = amount;
}

QString NativeTransactionEntry::category() const
{
   Q_ASSERT(m_hasCategory);
   return m_category;
}

bool NativeTransactionEntry::hasCategory() const
{
   return m_hasCategory;
}

void NativeTransactionEntry::setCategory(QString const& category)
{
   m_hasCategory = true;
   m_category = category;
}

QString NativeTransactionEntry::note() const
{
   Q_ASSERT(m_hasNote);
   return m_note;
}

bool NativeTransactionEntry::hasNote() const
{
   return m_hasNote;
}

void NativeTransactionEntry::setNote(QString const& note)
{
   m_hasNote = true;
   m_note = note;
}

QString NativeTransactionEntry::payee() const
{
   return m_payee;
}

void NativeTransactionEntry::setPayee(QString const& payee)
{
   m_payee = payee;
}
