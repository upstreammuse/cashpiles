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
#include "budgetcancelentry.h"
#include "budgetperiod.h"
#include "budgetuninitialized.h"
#include "categorynotexists.h"
#include "categoryownernotexists.h"
#include "transaction.h"
#include "referenceaccount.h"
#include "referencetransaction.h"
#include "rubbish.h"
#include "transaction.h"
#include "transactionaccountentry.h"
#include "transactioncategoryentry.h"
#include "transactioncategorytrackingentry.h"
#include "transactionnotexists.h"
#include "transactionownerentry.h"
#include "transactionownertrackingentry.h"

using namespace model;
using namespace util;

using std::find_if;
using std::logic_error;
using std::make_shared;
using std::shared_ptr;
using std::string;

auto Model::nextRange(DateRange const& range)
{
   size_t bigMonth = range.startDate.month();
   size_t day = range.startDate.day();
   size_t year = range.startDate.year();

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

auto Model::requireBudget(int id)
{
   if (!lastBudgetPeriod)
   {
      throw BudgetUninitialized();
   }
   if (lastBudgetPeriod->id != id)
   {
      throw Rubbish("Can only modify the most recent budget period");
   }
   return lastBudgetPeriod;
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

auto Model::requireTransaction(int id)
{
   auto it = transactions.find(id);
   if (it == transactions.end())
   {
      throw TransactionNotExists(id);
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

shared_ptr<BudgetPeriod const> Model::initializeBudget(
      Date const& date, Interval const& interval, string const& note)
{
   if (lastBudgetPeriod)
   {
      throw Rubbish("budget already initialized");
   }

   firstBudgetPeriod = lastBudgetPeriod = make_shared<BudgetPeriod>(
                                             DateRange(date, interval));
   lastBudgetPeriod->note = note;
   data.push_back(lastBudgetPeriod);
   return lastBudgetPeriod;
}

shared_ptr<BudgetPeriod const> Model::configureBudget(
      int id, Interval const& interval, string const& note)
{
   auto budget = requireBudget(id);
   budget->period = DateRange(budget->period.startDate, interval);
   budget->note = note;
   return move(budget);
}

shared_ptr<BudgetPeriod const> Model::growBudgetPeriods(Date const& date)
{
   if (!lastBudgetPeriod)
   {
      throw BudgetUninitialized();
   }

   while (nextRange(lastBudgetPeriod->period).startDate <= date)
   {
      auto nextPeriod = make_shared<BudgetPeriod>(
                           nextRange(lastBudgetPeriod->period));
      nextPeriod->prevPeriod = lastBudgetPeriod;
      lastBudgetPeriod->nextPeriod = nextPeriod;
      lastBudgetPeriod = nextPeriod;
   }
   return lastBudgetPeriod;
}

auto Model::requireGoalsCategory(std::string const& name)
{
   if (!categories.count(name))
   {
      throw CategoryNotExists(name);
   }
   auto category = goalCategories.find(name);
   if (category == goalCategories.end())
   {
      throw Rubbish("Category '" + name + "' is not a goals category");
   }
   return category->second;
}

auto Model::goalKey(std::string const& goalsCategory, std::string const& goal)
{
   return goalsCategory + "--" + goal;
}

auto Model::requireGoal(std::string const& category, std::string const& goal_)
{
   auto goalCategory = requireGoalsCategory(category);

   auto key = goalKey(category, goal_);
   auto goal = goals.find(key);
   if (goal == goals.end())
   {
      throw Rubbish("Goal '" + goal_ + "' in category '" + category +
                    "' does not exist");
   }
   return goal->second;
}

// TODO it probably makes sense for this to return an error if the current
// budget period doesn't have this category/goal combo, but that requires the
// ability to make sense of budget category logic to know when a goal is no
// longer valid
shared_ptr<BudgetCancelEntry const> Model::cancelGoal(
      std::string const& category, std::string const& goal_,
      std::string const& note)
{
   auto goal = requireGoal(category, goal_);
   auto cancel = make_shared<BudgetCancelEntry>(goal);
   cancel->note = note;
   data.push_back(cancel);
   return move(cancel);
}

shared_ptr<BudgetGoalEntry const> Model::budgetGoal(
      std::string const& category, std::string const& goal_)
{
   auto key = goalKey(category, goal_);
   auto goal = goals.find(key);
   if (goal == goals.end())
   {
      throw Rubbish("Goal '" + goal_ + "' in category '" + category +
                    "' not found");
   }
   return goal->second;
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
