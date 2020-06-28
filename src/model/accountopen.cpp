#include "accountopen.h"

using namespace model;

using std::string;

AccountOpen::AccountOpen(string const& account) :
   logic_error("Account '" + account + "' is already open"),
   m_account(account)
{
}

string AccountOpen::account() const
{
   return m_account;
}
