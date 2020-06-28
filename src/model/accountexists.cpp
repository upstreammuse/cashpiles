#include "accountexists.h"

using namespace model;

using std::string;

AccountExists::AccountExists(string const& account) :
   logic_error("Cannot open existing account '" + account + "'"),
   m_account(account)
{
}

string AccountExists::account() const
{
   return m_account;
}
