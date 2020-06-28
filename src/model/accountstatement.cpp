#include "accountstatement.h"

using namespace model;

AccountStatement::AccountStatement(
      std::shared_ptr<Account> account_, Date const& date_) :
   account(account_),
   date(date_)
{
}
