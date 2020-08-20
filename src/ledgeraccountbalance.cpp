#include "ledgeraccountbalance.h"

#include "itemprocessor.h"

using std::string;

LedgerAccountBalance::LedgerAccountBalance(
      Date const& date, std::string const& filename, size_t linenum) :
   LedgerItem(filename, linenum),
   m_date(date)
{
}

LedgerAccountBalance::LedgerAccountBalance(
      Date const& date, LedgerItem const& item) :
   LedgerItem(item),
   m_date(date)
{
}

string LedgerAccountBalance::account() const
{
   return m_account;
}

void LedgerAccountBalance::setAccount(string const& account)
{
   m_account = account;
}

Currency LedgerAccountBalance::amount() const
{
   return m_amount;
}

void LedgerAccountBalance::setAmount(Currency const& amount)
{
   m_amount = amount;
}

Date LedgerAccountBalance::date() const
{
   return m_date;
}

void LedgerAccountBalance::setDate(Date const& date)
{
   m_date = date;
}

void LedgerAccountBalance::processItem(ItemProcessor& processor) const
{
   processor.processItem(*this);
}
