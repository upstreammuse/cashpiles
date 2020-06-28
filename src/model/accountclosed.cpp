#include "accountclosed.h"

using namespace model;

using std::string;

AccountClosed::AccountClosed(string const& account) :
   logic_error("Account '" + account + "' is already closed"),
   m_account(account)
{
}

string AccountClosed::account() const
{
   return m_account;
}
