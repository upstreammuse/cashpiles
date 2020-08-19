#pragma once

#include <memory>
#include <vector>
#include "currency.h"
#include "date.h"
#include "ledgeritem.h"

// TODO propose the resurrection of the single-entry transaction for ynab import.  consider a 'checksum' hash that prevents the user from creating them (or a flag that has to be set to enable them), so that ynab can be imported directly, while at the same time coercing the user to use the V2 transactions for new things, and giving them the ability to migrate gradually if desired (or to leave it as-is 'legacy data')

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

   std::string payee() const;
   void setPayee(std::string const& payee);

   Status status() const;
   void setStatus(Status status);

   void processItem(ItemProcessor& processor) const;

private:
   std::string m_account;
   Currency m_amount;
   Date m_date;
   std::string m_payee;
   Status m_status = Status::PENDING;
};
