#include "ledgeraccountbalance.h"

#include "itemprocessor.h"

LedgerAccountBalance::LedgerAccountBalance()
{
}

LedgerAccountBalance::LedgerAccountBalance(QString const& fileName,
                                           int lineNum) :
   LedgerItem(fileName, lineNum)
{
}

QString LedgerAccountBalance::account() const
{
   return m_account;
}

Currency LedgerAccountBalance::amount() const
{
   return m_amount;
}

QDate LedgerAccountBalance::date() const
{
   return m_date;
}

void LedgerAccountBalance::setAccount(QString const& account)
{
   m_account = account;
}

void LedgerAccountBalance::setAmount(Currency const& amount)
{
   m_amount = amount;
}

void LedgerAccountBalance::setDate(QDate const& date)
{
   m_date = date;
}

void LedgerAccountBalance::processItem(ItemProcessor* processor)
{
   processor->processItem(*this);
}
