#include "accountwrongtype.h"

using namespace model;

using std::string;

AccountWrongType::AccountWrongType(string const& account) :
   logic_error("Account '" + account + "' is the wrong type."),
   m_account(account)
{
}

string AccountWrongType::account() const
{
   return m_account;
}
