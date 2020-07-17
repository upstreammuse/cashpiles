#pragma once

#include <stdexcept>
#include <string>

namespace model { class AccountNotExists; }

class model::AccountNotExists : public std::logic_error
{
public:
   explicit AccountNotExists(std::string const& account);
   std::string account() const;

private:
   std::string m_account;
};
