#include "ledgertransaction.h"

#include <cassert>
#include "itemprocessor.h"

LedgerTransaction::LedgerTransaction(
      Date const& date, std::string const& filename, size_t lineNum) :
   LedgerItem(filename, lineNum),
   m_date(date)
{
}

LedgerTransaction::LedgerTransaction(Date const& date, LedgerItem const& item) :
   LedgerItem(item),
   m_date(date)
{
}

std::string LedgerTransaction::account() const
{
   return m_account;
}

void LedgerTransaction::setAccount(std::string const& account)
{
   m_account = account;
}

Currency LedgerTransaction::amount() const
{
   return m_amount;
}

void LedgerTransaction::setAmount(Currency const& amount)
{
   m_amount = amount;
}

Date LedgerTransaction::date() const
{
   return m_date;
}

void LedgerTransaction::setDate(Date const& date)
{
   m_date = date;
}

std::string LedgerTransaction::payee() const
{
   return m_payee;
}

void LedgerTransaction::setPayee(std::string const& payee)
{
   m_payee = payee;
}

LedgerTransaction::Status LedgerTransaction::status() const
{
   return m_status;
}

void LedgerTransaction::setStatus(Status status)
{
   m_status = status;
}

void LedgerTransaction::processItem(ItemProcessor& processor) const
{
   processor.processItem(*this);
}
