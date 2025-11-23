#pragma once

#include "currency.h"
#include "date.h"
#include "ledgeritem.h"

class LedgerAccountBalance : public LedgerItem
{
public:
   using LedgerItem::LedgerItem;

   std::string account() const;
   void setAccount(std::string const& account);

   Currency amount() const;
   void setAmount(Currency const& amount);

   Date date() const;
   void setDate(Date const& date);

   void processItem(ItemProcessor& processor) const;

private:
   std::string m_account;
   Currency m_amount;
   Date m_date;
};
