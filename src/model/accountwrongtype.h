#pragma once

#include <stdexcept>
#include <string>

namespace model { class AccountWrongType; }

class model::AccountWrongType : public std::logic_error
{
public:
   explicit AccountWrongType(std::string const& account);
   std::string account() const;

private:
   std::string m_account;
};
