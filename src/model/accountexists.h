#pragma once

#include <stdexcept>

namespace model { class AccountExists; }

class model::AccountExists : public std::logic_error
{
public:
   explicit AccountExists(std::string const& account);
   std::string account() const;

private:
   std::string m_account;
};
