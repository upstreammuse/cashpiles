#include "nativeaccountcommand.h"

NativeAccountCommand::NativeAccountCommand(int item, int line) :
   NativeCommand(item, line)
{
}

QString NativeAccountCommand::account() const
{
   return m_account;
}

QString NativeAccountCommand::command() const
{
   return m_command;
}

QString NativeAccountCommand::date() const
{
   return m_date;
}

void NativeAccountCommand::setAccount(QString const& account)
{
   m_account = account;
}

void NativeAccountCommand::setCommand(QString const& command)
{
   m_command = command;
}

void NativeAccountCommand::setDate(QString const& date)
{
   m_date = date;
}
