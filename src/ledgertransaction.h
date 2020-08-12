#pragma once

#include <memory>
#include <vector>
#include "currency.h"
#include "date.h"
#include "ledgeritem.h"

// TODO rename "LedgerExternalTransaction" or similar
class LedgerTransaction : public LedgerItem
{
public:
   enum class Status
   {
      CLEARED,
      DISPUTED,
      PENDING
   };

public:
   LedgerTransaction(
         Date const& date, std::string const& filename, size_t lineNum);

   std::string account() const;
   void setAccount(std::string const& account);

   Currency amount() const;
   void setAmount(Currency const& amount);

   Date date() const;
   void setDate(Date const& date);

   std::pair<std::string, bool> note() const;
   void setNote(std::string const& note);

   std::string payee() const;
   void setPayee(std::string const& payee);

   Status status() const;
   void setStatus(Status status);

   void processItem(ItemProcessor& processor) const;

private:
   std::string m_account;
   Currency m_amount;
   Date m_date;
   std::pair<std::string, bool> m_note = std::make_pair("", false);
   std::string m_payee;
   Status m_status = Status::PENDING;
};
