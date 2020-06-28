#include "accountclosure.h"

using namespace model;

using std::shared_ptr;

AccountClosure::AccountClosure(shared_ptr<Account> account_) :
   account(account_)
{
}
