#include "transaction.h"

using namespace model;

using std::string;

Transaction::Transaction(
      std::shared_ptr<BudgetPeriod const> period, util::Date const& date_,
      TransactionFlag flag_, string const& payee_) :
   budgetPeriod(period),
   date(date_),
   flag(flag_),
   payee(payee_)
{
}
