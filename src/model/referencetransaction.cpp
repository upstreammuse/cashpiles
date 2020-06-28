#include "referencetransaction.h"

using namespace model;

using std::string;

ReferenceTransaction::ReferenceTransaction(
      std::shared_ptr<ReferenceAccount> account_, Date const& date_,
      string const& payee_, TransactionFlag flag) :
   account(account_),
   date(date_),
   payee(payee_),
   status(flag)
{
}
