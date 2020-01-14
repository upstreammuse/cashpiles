#ifndef LEDGERTRANSACTIONENTRY_H
#define LEDGERTRANSACTIONENTRY_H

#include <QString>
#include "currency.h"
#include "identifier.h"

class LedgerTransactionEntry
{
public:
   Currency amount() const;
   void setAmount(Currency const& amount);

   Identifier category() const;
   void setCategory(Identifier const& category);

   QString note() const;
   bool hasNote() const;
   void setNote(QString const& note);

   Identifier payee() const;
   void setPayee(Identifier const& payee);

private:
   Currency m_amount;
   Identifier m_category;
   QString m_note;
   bool m_hasNote = false;
   Identifier m_payee;
};

#endif
