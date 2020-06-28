#include "transactionflaginvalid.h"

using namespace model;

using std::string;

TransactionFlagInvalid::TransactionFlagInvalid(string const& flag) :
   logic_error("Transaction flag '" + flag + "' is invalid"),
   m_flag(flag)
{
}

string TransactionFlagInvalid::flag() const
{
   return m_flag;
}
