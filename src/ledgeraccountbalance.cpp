#include "ledgeraccountbalance.h"

#include "itemprocessor.h"

using std::string;

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
