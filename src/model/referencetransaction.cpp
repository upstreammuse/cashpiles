#include "referencetransaction.h"

using namespace model;

using std::string;

ReferenceTransaction::ReferenceTransaction(
      std::shared_ptr<ReferenceAccount const> account_, Date const& date_,
      TransactionFlag flag_, string const& payee_) :
   account(account_),
   date(date_),
   flag(flag_),
   payee(payee_)
{
}
