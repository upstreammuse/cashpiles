#include "ledgertransaction.h"

#include <cassert>
#include "itemprocessor.h"

LedgerTransaction::Status LedgerTransaction::statusFromString(
      std::string const& status, bool* ok)
{
   bool dummy;
   bool* success = ok ? ok : &dummy;
   *success = true;

   if (status == "*")
   {
      return Status::CLEARED;
   }
   else if (status == "?")
   {
      return Status::PENDING;
   }
   else if (status == "!")
   {
      return Status::DISPUTED;
   }
   else
   {
      *success = false;
      return Status::PENDING;
   }

}

std::string LedgerTransaction::statusToString(Status status)
{
   switch (status)
   {
      case Status::CLEARED:
         return "*";
      case Status::PENDING:
         return "?";
      case Status::DISPUTED:
         return "!";
   }

   assert("Status not handled" && false);
   return "";
}

LedgerTransaction::LedgerTransaction(std::string const& filename,
                                     size_t lineNum) :
   LedgerItem(filename, lineNum)
{
}

Identifier LedgerTransaction::account() const
{
   return m_account;
}

void LedgerTransaction::setAccount(Identifier const& account)
{
   m_account = account;
}

Currency LedgerTransaction::amount() const
{
   Currency amount;
   for (LedgerTransactionEntry const& entry : m_entries)
   {
      amount += entry.amount();
   }
   return amount;
}

Currency LedgerTransaction::balance() const
{
   assert(m_balance);
   return *m_balance;
}

bool LedgerTransaction::hasBalance() const
{
   return bool(m_balance);
}

void LedgerTransaction::setBalance(Currency const& balance)
{
   m_balance.reset(new Currency(balance));
}

Date LedgerTransaction::date() const
{
   return m_date;
}

void LedgerTransaction::setDate(Date const& date)
{
   m_date = date;
}

std::string LedgerTransaction::note() const
{
   assert(m_note);
   return *m_note;
}

bool LedgerTransaction::hasNote() const
{
   return m_note.get();
}

void LedgerTransaction::setNote(std::string const& note)
{
   m_note.reset(new std::string(note));
}

LedgerTransaction::Status LedgerTransaction::status() const
{
   return m_status;
}

void LedgerTransaction::setStatus(Status status)
{
   m_status = status;
}

void LedgerTransaction::appendEntry(LedgerTransactionEntry const& entry)
{
   m_entries.push_back(entry);
}

std::vector<LedgerTransactionEntry> const& LedgerTransaction::entries() const
{
   return m_entries;
}

void LedgerTransaction::processItem(ItemProcessor& processor) const
{
   processor.processItem(*this);
}
