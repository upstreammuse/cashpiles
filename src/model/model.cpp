#include "model.h"

#include <algorithm>
#include <cassert>
#include <map>
#include <memory>
#include <set>
#include <vector>
#include "account.h"
#include "accountclosed.h"
#include "accountclosure.h"
#include "accountexists.h"
#include "accountnotexists.h"
#include "accountopen.h"
#include "accountstatement.h"
#include "accountwrongtype.h"
#include "blank.h"
#include "budgetaccount.h"
#include "referenceaccount.h"
#include "referencetransaction.h"
#include "transaction.h"

using namespace model;

using std::find_if;
using std::logic_error;
using std::make_shared;
using std::shared_ptr;
using std::string;

// TODO everything come out of this namespace and be visible elsewhere
namespace {

struct Interval
{
   enum class Period
   {
      DAYS,
      MONTHS,
      YEARS
   };
   size_t number;
   Period period;

   Interval(size_t, Period);
};

struct DateRange
{
   Date startDate;
   Interval interval;

   DateRange(Date, Interval);
};

struct BudgetCategoryOwner : public ModelData
{
   std::string const name;  //PK
   Currency available;

   explicit BudgetCategoryOwner(std::string const&);
};

// TODO needs to be split out by type
struct BudgetCategory : public ModelData
{
   std::string const name;  //PK
   std::shared_ptr<BudgetCategoryOwner> owner;  //FK
   Currency balance;

   BudgetCategory(std::string const&, std::shared_ptr<BudgetCategoryOwner>);
};

struct BudgetPeriod : public ModelData
{
   DateRange const period;  //PK
   std::shared_ptr<BudgetPeriod> prevPeriod;  //FK-nillable
   std::shared_ptr<BudgetPeriod> nextPeriod;  //FK-nillable

   explicit BudgetPeriod(DateRange const&);
};

struct TransactionEntry : public ModelData
{
   int const id = newId();  //PK
   std::shared_ptr<Transaction> transaction;  //FK

   explicit TransactionEntry(std::shared_ptr<Transaction>);
};

struct TransactionEntryAccount : public TransactionEntry
{
   std::shared_ptr<BudgetAccount> account;  //FK
   std::shared_ptr<AccountStatement> statement;  //FK-nillable
   Currency amount;

   TransactionEntryAccount(std::shared_ptr<Transaction>, std::shared_ptr<Account>);
};

struct TransactionEntryCategory : public TransactionEntry
{
   std::shared_ptr<BudgetCategory> category;  //FK
   Currency amount;

   TransactionEntryCategory(std::shared_ptr<Transaction>, std::shared_ptr<BudgetCategory>);
};

struct TransactionEntryCategoryRef : public TransactionEntry
{
   std::shared_ptr<ReferenceAccount> account;  //FK
   std::shared_ptr<BudgetCategory> category;  //FK
   std::shared_ptr<AccountStatement> statement;  //FK-nillable
   Currency amount;

   TransactionEntryCategoryRef(std::shared_ptr<Transaction>, std::shared_ptr<ReferenceAccount>, std::shared_ptr<BudgetCategory>);
};

struct TransactionEntryOwner : public TransactionEntry
{
   std::shared_ptr<BudgetCategoryOwner> owner;  //FK
   Currency amount;

   TransactionEntryOwner(std::shared_ptr<Transaction>, std::shared_ptr<BudgetCategoryOwner>);
};

struct TransactionEntryOwnerRef : public TransactionEntry
{
   std::shared_ptr<ReferenceAccount> account;  //FK
   std::shared_ptr<BudgetCategoryOwner> owner;  //FK
   std::shared_ptr<AccountStatement> statement;  //FK-nillable
   Currency amount;

   TransactionEntryOwnerRef(std::shared_ptr<Transaction>, std::shared_ptr<ReferenceAccount>, std::shared_ptr<BudgetCategoryOwner>);
};

struct Comment : public ModelData
{
   int const id = newId();  //PK
   std::string content;

   explicit Comment(std::string const&);
};

// references to all data, represents file ordering
std::vector<std::shared_ptr<ModelData>> data;

// references to all accounts
std::set<std::shared_ptr<BudgetAccount>> budgetAccounts;
std::set<std::shared_ptr<ReferenceAccount>> referenceAccounts;

// references to all budget categories
std::set<std::shared_ptr<BudgetCategory>> categories;

// reverse access to statements from accounts
std::map<std::string, std::set<std::shared_ptr<AccountStatement>>> statements;

// reverse access to transactions from accounts
std::map<std::string, std::set<std::shared_ptr<TransactionEntryAccount>>>
budgetTransactions;
std::map<std::string, std::set<std::shared_ptr<ReferenceTransaction>>>
referenceTransactions;

// reverse access to transaction entries from statements
std::map<int, std::set<std::shared_ptr<TransactionEntry>>> entries;

// reverse access to transaction entries from transactions
std::map<int, std::set<std::shared_ptr<TransactionEntry>>> transactions;

// accesses to the budget period list
std::shared_ptr<BudgetPeriod> firstBudgetPeriod;
std::shared_ptr<BudgetPeriod> lastBudgetPeriod;

}

auto Model::requireBudgetAccount(string const& name)
{
   auto finder = [&](shared_ptr<Account> item) -> bool {
      return name == item->name;
   };
   auto it = find_if(budgetAccounts.begin(), budgetAccounts.end(), finder);
   auto otherIt = find_if(referenceAccounts.begin(), referenceAccounts.end(),
                          finder);
   assert(it == budgetAccounts.end() || otherIt == referenceAccounts.end());

   if (otherIt != referenceAccounts.end())
   {
      throw AccountWrongType(name);
   }
   else if (it == budgetAccounts.end())
   {
      throw AccountNotExists(name);
   }
   return *it;
}

auto Model::requireReferenceAccount(string const& name)
{
   auto finder = [&](shared_ptr<Account> item) -> bool {
      return name == item->name;
   };
   auto it = find_if(referenceAccounts.begin(), referenceAccounts.end(),
                     finder);
   auto otherIt = find_if(budgetAccounts.begin(), budgetAccounts.end(), finder);
   assert(it == referenceAccounts.end() || otherIt == budgetAccounts.end());

   if (otherIt != budgetAccounts.end())
   {
      throw AccountWrongType(name);
   }
   else if (it == referenceAccounts.end())
   {
      throw AccountNotExists(name);
   }
   return *it;
}

auto Model::requireAccount(string const& name)
{
   shared_ptr<Account> retval;
   int wrongCount = 0;
   try
   {
      retval = requireBudgetAccount(name);
      return retval;
   }
   catch (AccountWrongType)
   {
      ++wrongCount;
   }
   try
   {
      retval = requireReferenceAccount(name);
      return retval;
   }
   catch (AccountWrongType)
   {
      ++wrongCount;
   }
   assert(wrongCount < 2);
   throw AccountNotExists(name);
}

shared_ptr<BudgetAccount const> Model::createBudgetAccount(
      string const& name, string const& note)
{
   requireNoAccount(name);

   auto account = make_shared<BudgetAccount>(name);
   account->note = note;
   account->open = true;
   data.push_back(account);
   budgetAccounts.insert(account);
   return move(account);
}

shared_ptr<ReferenceAccount const> Model::createReferenceAccount(
      string const& name, string const& note)
{
   requireNoAccount(name);

   auto account = make_shared<ReferenceAccount>(name);
   account->note = note;
   account->open = true;
   data.push_back(account);
   referenceAccounts.insert(account);
   return move(account);
}

// TODO this is funny, since opening an account just means to ignore the
//   existence of accountclosure instances when writing the file back out
void Model::openAccount(string const& name)
{
   auto account = requireAccount(name);
   if (account->open)
   {
      throw AccountOpen(name);
   }
   account->open = true;
}

// TODO when writing closures for a closed account to the file, which one do you
//   write, the first one?  not sure why you'd do anything else
void Model::closeAccount(string const& name, string const& note)
{
   auto account = requireAccount(name);
   if (!account->open)
   {
      throw AccountClosed(name);
   }
   account->open = false;

   auto closure = make_shared<AccountClosure>(account);
   closure->note = note;
   data.push_back(closure);
}

shared_ptr<AccountStatement const> Model::createAccountStatement(
      Date const& date, string const& name, Currency const& balance,
      string const& note)
{
   auto account = requireAccount(name);

   auto statement = make_shared<AccountStatement>(account, date);
   statement->balance = balance;
   statement->note = note;
   data.push_back(statement);
   statements[name].insert(statement);
   return move(statement);
}

shared_ptr<Blank const> Model::createBlank(string const& note)
{
   auto blank = make_shared<Blank>();
   blank->note = note;
   data.push_back(blank);
   return move(blank);
}

shared_ptr<ReferenceTransaction const> Model::createReferenceTransaction(
      Date const& date, TransactionFlag flag, string const& account_,
      string const& payee, Currency const& amount, string const& note)
{
   auto account = requireReferenceAccount(account_);

   auto txn = make_shared<ReferenceTransaction>(account, date, flag, payee);
   txn->amount = amount;
   txn->note = note;
   data.push_back(txn);
   referenceTransactions[account_].insert(txn);
   return move(txn);
}

void Model::requireNoAccount(string const& name)
{
   try
   {
      requireAccount(name);
      throw AccountExists(name);
   }
   catch (AccountNotExists)
   {
   }
}
