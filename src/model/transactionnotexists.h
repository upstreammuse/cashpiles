#pragma once

#include <stdexcept>

namespace model { class TransactionNotExists; }

class model::TransactionNotExists : public std::logic_error
{
public:
   explicit TransactionNotExists(int);
   int transaction() const;

private:
   static auto formatHelper(int);

private:
   int m_transaction;
};
