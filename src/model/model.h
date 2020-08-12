#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>
#include "../currency.h"
#include "../date.h"
#include "../interval.h"

namespace model { struct Account; }
namespace model { struct AccountStatement; }
namespace model { struct Blank; }
namespace model { struct BudgetAccount; }
namespace model { struct BudgetCancelEntry; }
namespace model { struct BudgetCategoryEntry; }
namespace model { struct BudgetCategoryOwner; }
namespace model { struct BudgetGoalEntry; }
namespace model { struct BudgetGoalsEntry; }
namespace model { struct BudgetIncomeEntry; }
namespace model { struct BudgetPeriod; }
namespace model { struct DateRange; }
namespace model { class Model; }
namespace model { struct ModelData; }
namespace model { struct ReferenceAccount; }
namespace model { struct ReferenceTransaction; }
namespace model { struct Transaction; }
namespace model { struct TransactionAccountEntry; }
namespace model { struct TransactionCategoryEntry; }
namespace model { struct TransactionCategoryTrackingEntry; }
namespace model { struct TransactionOwnerEntry; }
namespace model { struct TransactionOwnerTrackingEntry; }
namespace model { enum class TransactionFlag; }

class model::Model
{
public:
   enum class IdentifierType
   {
      ACCOUNT,
      CATEGORY,
      OWNER
   };

public:
   // general stuff
   IdentifierType getIdentifierType(std::string const&);

   // accounts
   // create
   std::shared_ptr<BudgetAccount const> createBudgetAccount(
         std::string const& name, std::string const& note);
   std::shared_ptr<ReferenceAccount const> createReferenceAccount(
         std::string const& name, std::string const& note);
   // read
   std::shared_ptr<Account const> getAccount(std::string const&);
   std::shared_ptr<BudgetAccount const> getBudgetAccount(std::string const&);
   std::shared_ptr<ReferenceAccount const> getReferenceAccount(
         std::string const&);
   // update
   void openAccount(std::string const&);
   void closeAccount(std::string const& name, std::string const& note);
   // delete
//   void deleteAccount(std::string const&);

   // account statements
   // create
   std::shared_ptr<AccountStatement const> createAccountStatement(
         Date const&, std::string const& name, Currency const&,
         std::string const& note);
   // read
   // update
   // delete

   // blank lines and comments
   // create
   std::shared_ptr<Blank const> createBlank(std::string const& note);
   // read
   // update
   // delete

   // budget
   // create
   std::shared_ptr<BudgetPeriod const> initializeBudget(
         Date const&, Interval const&, std::string const& note);
   // read
   std::shared_ptr<BudgetPeriod const> getBudget(int);
   std::shared_ptr<BudgetPeriod const> getCurrentBudget();
   // update
   std::shared_ptr<BudgetPeriod const> configureBudget(
         int, Interval const&, std::string const& note);
   std::shared_ptr<BudgetPeriod const> growBudgetPeriods(Date const&);
   // delete







   // create, update, and grow budget
   std::shared_ptr<BudgetCancelEntry const> cancelGoal(
         std::string const& category, std::string const& goal,
         std::string const& note);

   // create budget items
   std::shared_ptr<BudgetIncomeEntry const> createBudgetIncomeEntry(
         int, std::string const& name, std::string const& owner,
         std::string const& note);

   // read budget items
//   std::shared_ptr<BudgetCategoryEntry const> budgetCategory(
//         std::string const& category);
   std::shared_ptr<BudgetGoalEntry const> budgetGoal(
         std::string const& category, std::string const& goal);

   // create reference transactions
   std::shared_ptr<ReferenceTransaction const> createReferenceTransaction(
         Date const&, TransactionFlag, std::string const& account,
         std::string const& payee, Currency const&, std::string const& note);

   // create transactions
   std::shared_ptr<Transaction const> createTransaction(
         Date const&, TransactionFlag, std::string const& payee,
         std::string const& note);
   std::shared_ptr<TransactionAccountEntry const> createAccountEntry(
         int, std::string const& name, Currency const&,
         std::string const& note);
   std::shared_ptr<TransactionCategoryEntry const> createCategoryEntry(
         int, std::string const& name, Currency const&,
         std::string const& note);
   std::shared_ptr<TransactionCategoryTrackingEntry const>
   createCategoryTrackingEntry(
         int, std::string const& name, std::string const& account,
         Currency const&, std::string const& note);
   std::shared_ptr<TransactionOwnerEntry const> createOwnerEntry(
         int, std::string const& name, Currency const&,
         std::string const& note);
   std::shared_ptr<TransactionOwnerTrackingEntry const>
   createOwnerTrackingEntry(
         int, std::string const& name, std::string const& account,
         Currency const&, std::string const& note);
   void finalizeTransaction(int);

private:
   auto nextRange(DateRange const&);
   auto requireBudgetCategory(std::string const&);
   auto requireBudgetCategoryOwner(std::string const&);
   auto requireGoal(std::string const& category, std::string const& goal);
   auto requireGoalsCategory(std::string const&);
   void requireNoAccount(std::string const&);
   auto requireTransaction(int);
   void requireUnusedIdentifier(std::string const&);

private:
   std::vector<std::shared_ptr<ModelData>> data;
   std::map<std::string, IdentifierType> identifiers;

   std::map<std::string, std::shared_ptr<BudgetAccount>> budgetAccounts;
   std::map<std::string, std::shared_ptr<ReferenceAccount>> referenceAccounts;

   std::map<int, std::shared_ptr<AccountStatement>> statements;

   std::shared_ptr<BudgetPeriod> firstBudgetPeriod;
   std::shared_ptr<BudgetPeriod> lastBudgetPeriod;
   std::map<std::string, std::shared_ptr<BudgetCategoryEntry>> categories;
   std::map<std::string,
   std::map<std::string, std::shared_ptr<BudgetGoalEntry>>> goals;
   std::map<std::string, std::shared_ptr<BudgetIncomeEntry>> incomes;
   std::map<std::string, std::shared_ptr<BudgetCategoryOwner>> owners;

   std::map<int, std::shared_ptr<Transaction>> transactions;
};
