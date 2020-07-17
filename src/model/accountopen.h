#pragma once

#include <stdexcept>
#include <string>

namespace model { class AccountOpen; }

class model::AccountOpen : public std::logic_error
{
public:
   explicit AccountOpen(std::string const& account);
   std::string account() const;

private:
   std::string m_account;
};
