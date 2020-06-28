#include "accountnotexists.h"

using namespace model;

using std::string;

AccountNotExists::AccountNotExists(string const& account) :
   logic_error("Account '" + account + "' does not exist"),
   m_account(account)
{
}

string AccountNotExists::account() const
{
   return m_account;
}
