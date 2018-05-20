#include "ledgeraccountcommand.h"

LedgerAccountCommand::LedgerAccountCommand(int seqNum, int lineNum) :
   LedgerCommand(seqNum, lineNum)
{
}

QString LedgerAccountCommand::account() const
{
   return m_account;
}

QDate LedgerAccountCommand::date() const
{
   return m_date;
}

LedgerAccountCommand::Mode LedgerAccountCommand::mode() const
{
   return m_mode;
}

void LedgerAccountCommand::setAccount(QString const& account)
{
   m_account = account;
}

void LedgerAccountCommand::setDate(QDate const& date)
{
   m_date = date;
}

void LedgerAccountCommand::setMode(Mode mode)
{
   m_mode = mode;
}
