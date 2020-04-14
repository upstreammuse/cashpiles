#pragma once

#include <string>
#include "currency.h"
#include "identifier.h"

class LedgerTransactionEntry
{
public:
   Currency amount() const;
   void setAmount(Currency const& amount);

   Identifier category() const;
   void setCategory(Identifier const& category);

   std::string note() const;
   bool hasNote() const;
   void setNote(std::string const& note);

   Identifier payee() const;
   void setPayee(Identifier const& payee);

private:
   Currency m_amount;
   Identifier m_category;
   std::string m_note;
   bool m_hasNote = false;
   Identifier m_payee;
};
