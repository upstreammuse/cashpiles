#pragma once

#include <memory>
#include <string>
#include "m_currency.h"
#include "m_date.h"

namespace model { struct Account; }
namespace model { struct AccountStatement; }
namespace model { struct Blank; }
namespace model { struct BudgetAccount; }
namespace model { class Model; }
namespace model { struct ReferenceAccount; }
namespace model { struct ReferenceTransaction; }
namespace model { struct Transaction; }
namespace model { struct TransactionAccountLine; }
namespace model { struct TransactionCategoryLine; }
namespace model { struct TransactionCategoryTrackingLine; }
namespace model { struct TransactionOwnerLine; }
namespace model { struct TransactionOwnerTrackingLine; }
namespace model { enum class TransactionFlag; }

class model::Model
{
public:
   std::shared_ptr<BudgetAccount const> createBudgetAccount(
         std::string const& name, std::string const& note);
   std::shared_ptr<ReferenceAccount const> createReferenceAccount(
         std::string const& name, std::string const& note);
   void openAccount(std::string const& name);
   void closeAccount(std::string const& name, std::string const& note);

   std::shared_ptr<AccountStatement const> createAccountStatement(
         Date const&, std::string const& name, Currency const&,
         std::string const& note);

   std::shared_ptr<Blank const> createBlank(std::string const& note);

   std::shared_ptr<ReferenceTransaction const> createReferenceTransaction(
         Date const&, TransactionFlag, std::string const& account,
         std::string const& payee, Currency const&, std::string const& note);

   std::shared_ptr<Transaction const> createTransaction(
         Date const&, TransactionFlag, std::string const& payee,
         std::string const& note);
   std::shared_ptr<TransactionAccountLine const> createAccountLine(
         int, std::string const& name, Currency const&,
         std::string const& note);
   std::shared_ptr<TransactionCategoryLine const> createCategoryLine(
         int, std::string const& name, Currency const&,
         std::string const& note);
   std::shared_ptr<TransactionCategoryTrackingLine const>
   createCategoryTrackingLine(
         int, std::string const& name, std::string const& account,
         Currency const&, std::string const& note);
   std::shared_ptr<TransactionOwnerLine const> createOwnerLine(
         int, std::string const& name, Currency const&,
         std::string const& note);
   std::shared_ptr<TransactionOwnerTrackingLine const> createOwnerTrackingLine(
         int, std::string const& name, std::string const& account,
         Currency const&, std::string const& note);
   void finalizeTransaction(int);

private:
   auto requireAccount(std::string const&);
   auto requireBudgetAccount(std::string const&);
   void requireNoAccount(std::string const&);
   auto requireReferenceAccount(std::string const&);
};
