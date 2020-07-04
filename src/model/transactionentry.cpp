#include "transactionentry.h"

using namespace model;

using std::shared_ptr;

TransactionEntry::TransactionEntry(shared_ptr<Transaction const> transaction_) :
   transaction(transaction_)
{
}
