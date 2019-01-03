#include "ledgeraccountcommand.h"

#include "itemprocessor.h"

LedgerAccountCommand::LedgerAccountCommand(QString const& filename,
                                           int lineNum) :
   LedgerItem(filename, lineNum)
{
}

QString LedgerAccountCommand::account() const
{
   return m_account;
}

void LedgerAccountCommand::setAccount(QString const& account)
{
   m_account = account;
}

QDate LedgerAccountCommand::date() const
{
   return m_date;
}

void LedgerAccountCommand::setDate(QDate const& date)
{
   m_date = date;
}

LedgerAccountCommand::Mode LedgerAccountCommand::mode() const
{
   return m_mode;
}

void LedgerAccountCommand::setMode(Mode mode)
{
   m_mode = mode;
}

void LedgerAccountCommand::processItem(ItemProcessor* processor)
{
   processor->processItem(*this);
}
