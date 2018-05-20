#ifndef NATIVEACCOUNTCOMMAND_H
#define NATIVEACCOUNTCOMMAND_H

#include <QString>
#include "nativecommand.h"

class NativeAccountCommand : public NativeCommand
{
public:
   NativeAccountCommand(int item, int line);
   QString account() const;
   QString command() const;
   QString date() const;
   void setAccount(QString const& account);
   void setCommand(QString const& command);
   void setDate(QString const& date);

private:
   QString m_account;
   QString m_command;
   QString m_date;
};

#endif
