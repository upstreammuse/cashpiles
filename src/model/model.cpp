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
#include "budgetperiod.h"
#include "budgetuninitialized.h"
#include "categorynotexists.h"
#include "categoryownernotexists.h"
#include "transaction.h"
#include "referenceaccount.h"
#include "referencetransaction.h"
#include "transaction.h"
#include "transactionaccountentry.h"
#include "transactioncategoryentry.h"
#include "transactioncategorytrackingentry.h"
#include "transactionnotexists.h"
#include "transactionownerentry.h"
#include "transactionownertrackingentry.h"

using namespace model;

using std::find_if;
using std::logic_error;
using std::make_shared;
using std::shared_ptr;
using std::string;

// TODO everything come out of this namespace and be visible elsewhere
#if 0
namespace {

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

// references to all budget categories
std::set<std::shared_ptr<BudgetCategory>> categories;

// reverse access to statements from accounts
//std::map<std::string, std::set<std::shared_ptr<AccountStatement>>> statements;

// reverse access to transactions from accounts
//std::map<std::string, std::set<std::shared_ptr<TransactionEntryAccount>>>
//budgetTransactions;
//std::map<std::string, std::set<std::shared_ptr<ReferenceTransaction>>>
//referenceTransactions;

// reverse access to transaction entries from statements
// TODO how to get to ref trans from statement?
//std::map<int, std::set<std::shared_ptr<TransactionEntry>>> entries;

// reverse access to transaction entries from transactions
//std::map<int, std::set<std::shared_ptr<TransactionEntry>>> transactions;

}
#endif

auto Model::compareDates(Date const& left, Date const& right)
{
   if (left.year < right.year)
   {
      return -1;
   }
   else if (left.year > right.year)
   {
      return 1;
   }
   else
   {
      if (left.month < right.month)
      {
         return -1;
      }
      else if (left.month > right.month)
      {
         return 1;
      }
      else
      {
         if (left.day < right.day)
         {
            return -1;
         }
         else if (left.day > right.day)
         {
            return 1;
         }
         else
         {
            return 0;
         }
      }
   }
}

auto Model::nextRange(DateRange const& range)
{
   size_t bigMonth = range.startDate.month;
   size_t day = range.startDate.day;
   size_t year = range.startDate.year;

   switch (range.interval.period)
   {
      case Interval::Period::DAYS:
         day += range.interval.number;
         break;
      case Interval::Period::MONTHS:
         bigMonth += range.interval.number;
         break;
      case Interval::Period::YEARS:
         year += range.interval.number;
         break;
   }

   while (bigMonth > 12)
   {
      bigMonth -= 12;
      ++year;
   }

   unsigned char month = static_cast<unsigned char>(bigMonth);

   while (day > Date::daysInMonth(month, year))
   {
      day -= Date::daysInMonth(month, year);
      ++month;
      if (month > 12)
      {
         month -= 12;
         ++year;
      }
   }

   return DateRange(Date(month, static_cast<unsigned char>(day), year),
                    range.interval);
}

auto Model::requireBudgetAccount(string const& name)
{
   auto it = budgetAccounts.find(name);
   auto otherIt = referenceAccounts.find(name);
   assert(it == budgetAccounts.end() || otherIt == referenceAccounts.end());

   if (otherIt != referenceAccounts.end())
   {
      throw AccountWrongType(name);
   }
   else if (it == budgetAccounts.end())
   {
      throw AccountNotExists(name);
   }
   return it->second;
}

auto Model::requireReferenceAccount(string const& name)
{
   auto it = referenceAccounts.find(name);
   auto otherIt = budgetAccounts.find(name);
   assert(it == referenceAccounts.end() || otherIt == budgetAccounts.end());

   if (otherIt != budgetAccounts.end())
   {
      throw AccountWrongType(name);
   }
   else if (it == referenceAccounts.end())
   {
      throw AccountNotExists(name);
   }
   return it->second;
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

auto Model::requireBudgetCategory(string const& name)
{
   auto it = categories.find(name);
   if (it == categories.end())
   {
      throw CategoryNotExists(name);
   }

   return it->second;
}

auto Model::requireBudgetCategoryOwner(string const& name)
{
   auto it = owners.find(name);
   if (it == owners.end())
   {
      throw CategoryOwnerNotExists(name);
   }

   return it->second;
}

shared_ptr<BudgetAccount const> Model::createBudgetAccount(
      string const& name, string const& note)
{
   requireNoAccount(name);

   auto account = make_shared<BudgetAccount>(name);
   account->note = note;
   account->open = true;
   data.push_back(account);
   budgetAccounts[name] = account;
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
   referenceAccounts[name] = account;
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
   return move(statement);
}

shared_ptr<Blank const> Model::createBlank(string const& note)
{
   auto blank = make_shared<Blank>();
   blank->note = note;
   data.push_back(blank);
   return move(blank);
}

shared_ptr<BudgetPeriod const> Model::growBudgetPeriods(Date const& date)
{
   if (!lastBudgetPeriod)
   {
      throw BudgetUninitialized();
   }

   while (compareDates(nextRange(lastBudgetPeriod->period).startDate,
                       date) <= 0)
   {
      auto nextPeriod = make_shared<BudgetPeriod>(
                           nextRange(lastBudgetPeriod->period));
      nextPeriod->prevPeriod = lastBudgetPeriod;
      lastBudgetPeriod->nextPeriod = nextPeriod;
      lastBudgetPeriod = nextPeriod;
   }
   return lastBudgetPeriod;
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
   return move(txn);
}

shared_ptr<Transaction const> Model::createTransaction(
      Date const& date, TransactionFlag flag, string const& payee,
      string const& note)
{
   auto budgetPeriod = growBudgetPeriods(date);

   auto txn = make_shared<Transaction>(budgetPeriod, date, flag, payee);
   txn->note = note;
   data.push_back(txn);
   transactions[txn->id] = txn;
   return move(txn);
}

// TODO move this
auto Model::requireTransaction(int id)
{
   auto it = transactions.find(id);
   if (it == transactions.end())
   {
      throw TransactionNotExists(id);
   }
   return it->second;
}

shared_ptr<TransactionAccountEntry const> Model::createAccountEntry(
      int id, std::string const& name, Currency const& amount,
      std::string const& note)
{
   auto txn = requireTransaction(id);
   auto account = requireBudgetAccount(name);

   auto entry = make_shared<TransactionAccountEntry>(txn, account);
   entry->amount = amount;
   entry->note = note;
   data.push_back(entry);
   return move(entry);
}

shared_ptr<TransactionCategoryEntry const> Model::createCategoryEntry(
      int id, std::string const& name, Currency const& amount,
      std::string const& note)
{
   auto txn = requireTransaction(id);
   auto category = requireBudgetCategory(name);

   auto entry = make_shared<TransactionCategoryEntry>(txn, category);
   entry->amount = amount;
   entry->note = note;
   data.push_back(entry);
   return move(entry);
}

shared_ptr<TransactionCategoryTrackingEntry const>
Model::createCategoryTrackingEntry(
      int id, std::string const& name, std::string const& account_,
      Currency const& amount, std::string const& note)
{
   auto txn = requireTransaction(id);
   auto category = requireBudgetCategory(name);
   auto account = requireReferenceAccount(account_);

   auto entry = make_shared<TransactionCategoryTrackingEntry>(
                   txn, account, category);
   entry->amount = amount;
   entry->note = note;
   data.push_back(entry);
   return move(entry);
}

shared_ptr<TransactionOwnerEntry const> Model::createOwnerEntry(
      int id, std::string const& name, Currency const& amount,
      std::string const& note)
{
   auto txn = requireTransaction(id);
   auto owner = requireBudgetCategoryOwner(name);

   auto entry = make_shared<TransactionOwnerEntry>(txn, owner);
   entry->amount = amount;
   entry->note = note;
   data.push_back(entry);
   return move(entry);
}

shared_ptr<TransactionOwnerTrackingEntry const>
Model::createOwnerTrackingEntry(
      int id, std::string const& name, std::string const& account_,
      Currency const& amount, std::string const& note)
{
   auto txn = requireTransaction(id);
   auto owner = requireBudgetCategoryOwner(name);
   auto account = requireReferenceAccount(account_);

   auto entry = make_shared<TransactionOwnerTrackingEntry>(txn, account, owner);
   entry->amount = amount;
   entry->note = note;
   data.push_back(entry);
   return move(entry);
}

void Model::finalizeTransaction(int id)
{
   auto txn = requireTransaction(id);
   // TODO get entries for this txn
   // TODO balance owners and categories with accounts
   // TODO etc.
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
