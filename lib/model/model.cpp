#include "model/model.h"

#include <QFile>

model::Model::Model(QString const& fileName) :
   m_fileName(fileName)
{
   QFile file(m_fileName);
   file.open(QIODevice::ReadOnly | QIODevice::Text);
   QTextStream in(&file);

   uint numAccounts;
   in >> numAccounts;
   for (uint i = 0; i < numAccounts; ++i)
   {
      Account account;
      in >> account;
      m_accounts[account.name] = account;
   }

   uint numAccountClosures;
   in >> numAccountClosures;
   for (uint i = 0; i < numAccountClosures; ++i)
   {
      AccountClosure accountClosure;
      in >> accountClosure;
      m_accountClosures[accountClosure.account] = accountClosure;
   }
}

model::Model::~Model()
{
   QFile file(m_fileName);
   file.open(QIODevice::WriteOnly | QIODevice::Text);
   QTextStream out(&file);

   out << m_accounts.size() << '\n';
   foreach (Account const& account, m_accounts)
   {
      out << account;
   }

   out << m_accountClosures.size() << '\n';
   foreach (AccountClosure const& accountClosure, m_accountClosures)
   {
      out << accountClosure;
   }
}
