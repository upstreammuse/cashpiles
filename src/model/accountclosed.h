#pragma once

#include <stdexcept>
#include <string>

namespace model { class AccountClosed; }

class model::AccountClosed : public std::logic_error
{
public:
   explicit AccountClosed(std::string const& account);
   std::string account() const;

private:
   std::string m_account;
};
