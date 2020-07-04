#include "transactionnotexists.h"

#include <sstream>

using namespace model;

using std::stringstream;

auto TransactionNotExists::formatHelper(int id)
{
   stringstream ss;
   ss << id;
   return ss.str();
}

TransactionNotExists::TransactionNotExists(int id) :
   logic_error("Transaction " + formatHelper(id) + " does not exist"),
   m_transaction(id)
{
}

int TransactionNotExists::transaction() const
{
   return m_transaction;
}
