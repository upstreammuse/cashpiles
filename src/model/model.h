#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>
#include "../util/m_currency.h"
#include "../util/m_date.h"

namespace model { struct Account; }
namespace model { struct AccountStatement; }
namespace model { struct Blank; }
namespace model { struct BudgetAccount; }
namespace model { struct BudgetCancelEntry; }
namespace model { struct BudgetCategoryEntry; }
namespace model { struct BudgetCategoryOwner; }
namespace model { struct BudgetGoalEntry; }
namespace model { struct BudgetGoalsEntry; }
namespace model { struct BudgetPeriod; }
namespace model { struct DateRange; }
namespace model { struct Interval; }
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
   std::shared_ptr<BudgetAccount const> createBudgetAccount(
         std::string const& name, std::string const& note);
   std::shared_ptr<ReferenceAccount const> createReferenceAccount(
         std::string const& name, std::string const& note);
   void openAccount(std::string const& name);
   void closeAccount(std::string const& name, std::string const& note);

   std::shared_ptr<AccountStatement const> createAccountStatement(
         util::Date const&, std::string const& name, util::Currency const&,
         std::string const& note);

   std::shared_ptr<Blank const> createBlank(std::string const& note);

   std::shared_ptr<BudgetPeriod const> initializeBudget(
         util::Date const&, Interval const&, std::string const& note);
   std::shared_ptr<BudgetPeriod const> configureBudget(
         int, Interval const&, std::string const& note);
   std::shared_ptr<BudgetPeriod const> growBudgetPeriods(util::Date const&);
   std::shared_ptr<BudgetCancelEntry const> cancelGoal(
         std::string const& category, std::string const& goal,
         std::string const& note);

   std::shared_ptr<BudgetCategoryEntry const> budgetCategory(
         std::string const& category);
   std::shared_ptr<BudgetGoalEntry const> budgetGoal(
         std::string const& category, std::string const& goal);
   std::shared_ptr<BudgetPeriod const> currentBudget();

   std::shared_ptr<ReferenceTransaction const> createReferenceTransaction(
         util::Date const&, TransactionFlag, std::string const& account,
         std::string const& payee, util::Currency const&,
         std::string const& note);

   std::shared_ptr<Transaction const> createTransaction(
         util::Date const&, TransactionFlag, std::string const& payee,
         std::string const& note);
   std::shared_ptr<TransactionAccountEntry const> createAccountEntry(
         int, std::string const& name, util::Currency const&,
         std::string const& note);
   std::shared_ptr<TransactionCategoryEntry const> createCategoryEntry(
         int, std::string const& name, util::Currency const&,
         std::string const& note);
   std::shared_ptr<TransactionCategoryTrackingEntry const>
   createCategoryTrackingEntry(
         int, std::string const& name, std::string const& account,
         util::Currency const&, std::string const& note);
   std::shared_ptr<TransactionOwnerEntry const> createOwnerEntry(
         int, std::string const& name, util::Currency const&,
         std::string const& note);
   std::shared_ptr<TransactionOwnerTrackingEntry const>
   createOwnerTrackingEntry(
         int, std::string const& name, std::string const& account,
         util::Currency const&, std::string const& note);
   void finalizeTransaction(int);

private:
   auto goalKey(std::string const& goalsCategory, std::string const& goal);
   auto nextRange(DateRange const&);
   auto requireAccount(std::string const&);
   auto requireBudget(int);
   auto requireBudgetAccount(std::string const&);
   auto requireBudgetCategory(std::string const&);
   auto requireBudgetCategoryOwner(std::string const&);
   auto requireGoal(std::string const& category, std::string const& goal);
   auto requireGoalsCategory(std::string const&);
   void requireNoAccount(std::string const&);
   auto requireReferenceAccount(std::string const&);
   auto requireTransaction(int);

private:
   std::vector<std::shared_ptr<ModelData>> data;

   std::map<std::string, std::shared_ptr<BudgetAccount>> budgetAccounts;
   std::map<std::string, std::shared_ptr<ReferenceAccount>> referenceAccounts;

   std::shared_ptr<BudgetPeriod> firstBudgetPeriod;
   std::shared_ptr<BudgetPeriod> lastBudgetPeriod;
   std::map<std::string, std::shared_ptr<BudgetCategoryEntry>> categories;
   std::map<std::string, std::shared_ptr<BudgetGoalsEntry>> goalCategories;
   std::map<std::string, std::shared_ptr<BudgetGoalEntry>> goals;
   std::map<std::string, std::shared_ptr<BudgetCategoryOwner>> owners;

   std::map<int, std::shared_ptr<Transaction>> transactions;
};
