#pragma once

#include <stdexcept>

namespace model { class TransactionFlagInvalid; }

class model::TransactionFlagInvalid : public std::logic_error
{
public:
   explicit TransactionFlagInvalid(std::string const& flag);
   std::string flag() const;

private:
   std::string m_flag;
};
