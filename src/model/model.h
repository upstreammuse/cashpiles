#pragma once

#include <string>
#include "m_currency.h"
#include "m_date.h"

namespace model { class Model; }
namespace model { enum class TransactionFlag; }

class model::Model
{
public:
   void createBudgetAccount(std::string const& name, std::string const& note);
   void createReferenceAccount(
         std::string const& name, std::string const& note);
   void openAccount(std::string const&);
   void closeAccount(std::string const& name, std::string const& note);

   void createAccountStatement(
         Date const&, std::string const& name, Currency const&,
         std::string const& note);

   void createBlank(std::string const& note);

   void createReferenceTransaction(
         Date const&, TransactionFlag, std::string const& account,
         std::string const& payee, Currency const&, std::string const& note);

private:
   auto requireAccount(std::string const&);
   auto requireBudgetAccount(std::string const&);
   void requireNoAccount(std::string const&);
   auto requireReferenceAccount(std::string const&);
};
