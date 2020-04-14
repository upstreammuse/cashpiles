#pragma once

#include <memory>
#include <vector>
#include "date.h"
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
   static std::string statusToString(Status status);

public:
   LedgerTransaction(std::string const& filename, size_t lineNum);

   Identifier account() const;
   void setAccount(Identifier const& account);

   Currency amount() const;

   Currency balance() const;
   bool hasBalance() const;
   void setBalance(Currency const& balance);

   Date date() const;
   void setDate(Date const& date);

   std::string note() const;
   bool hasNote() const;
   void setNote(std::string const& note);

   Status status() const;
   void setStatus(Status status);

   void appendEntry(LedgerTransactionEntry const& entry);
   std::vector<LedgerTransactionEntry> const& entries() const;

   void processItem(ItemProcessor& processor) const;

private:
   Identifier m_account;
   std::shared_ptr<Currency> m_balance;
   Date m_date;
   std::vector<LedgerTransactionEntry> m_entries;
   std::shared_ptr<std::string> m_note;
   Status m_status = PENDING;
};
