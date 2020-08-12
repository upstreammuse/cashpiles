#include "ledgertransactionv2.h"

#include <cassert>
#include "cashpiles.h"
#include "itemprocessor.h"

using std::pair;
using std::shared_ptr;
using std::string;

pair<Currency, bool> LedgerTransactionV2Entry::amount() const
{
   return m_amount;
}

void LedgerTransactionV2Entry::setAmount(Currency const& amount)
{
   m_amount.first = amount;
   m_amount.second = true;
}

pair<string, bool> LedgerTransactionV2Entry::note() const
{
   return m_note;
}

void LedgerTransactionV2Entry::setNote(string const& note)
{
   m_note.first = note;
   m_note.second = true;
}

string LedgerTransactionV2AccountEntry::account() const
{
   return m_account;
}

void LedgerTransactionV2AccountEntry::setAccount(string const& account)
{
   m_account = account;
}

void LedgerTransactionV2AccountEntry::balance(
      Currency& accounts, Currency&) const
{
   if (amount().second)
   {
      accounts += amount().first;
   }
}

void LedgerTransactionV2AccountEntry::setBalance(
      Currency& accounts, Currency& categories)
{
   if (!amount().second)
   {
      setAmount(categories - accounts);
      accounts += amount().first;
   }
}

void LedgerTransactionV2AccountEntry::processItem(
      ItemProcessor& processor) const
{
   processor.processItem(*this);
}

void LedgerTransactionV2CategoryEntry::balance(
      Currency&, Currency& categories) const
{
   if (amount().second)
   {
      categories += amount().first;
   }
}

void LedgerTransactionV2CategoryEntry::setBalance(
      Currency& accounts, Currency& categories)
{
   if (!amount().second)
   {
      setAmount(accounts - categories);
      categories += amount().first;
   }
}

string LedgerTransactionV2CategoryEntry::category() const
{
   return m_category;
}

void LedgerTransactionV2CategoryEntry::setCategory(string const& category)
{
   m_category = category;
}

pair<string, bool> LedgerTransactionV2CategoryEntry::trackingAccount() const
{
   return m_trackingAccount;
}

void LedgerTransactionV2CategoryEntry::setTrackingAccount(string const& account)
{
   m_trackingAccount.first = account;
   m_trackingAccount.second = true;
}

void LedgerTransactionV2CategoryEntry::processItem(
      ItemProcessor& processor) const
{
   processor.processItem(*this);
}

void LedgerTransactionV2OwnerEntry::balance(
      Currency&, Currency& categories) const
{
   if (amount().second)
   {
      categories += amount().first;
   }
}

void LedgerTransactionV2OwnerEntry::setBalance(
      Currency& accounts, Currency& categories)
{
   if (!amount().second)
   {
      setAmount(accounts - categories);
      categories += amount().first;
   }
}

string LedgerTransactionV2OwnerEntry::owner() const
{
   return m_owner;
}

void LedgerTransactionV2OwnerEntry::setOwner(string const& owner)
{
   m_owner = owner;
}

pair<string, bool> LedgerTransactionV2OwnerEntry::trackingAccount() const
{
   return m_trackingAccount;
}

void LedgerTransactionV2OwnerEntry::setTrackingAccount(string const& account)
{
   m_trackingAccount = make_pair(account, true);
}

void LedgerTransactionV2OwnerEntry::processItem(ItemProcessor& processor) const
{
   processor.processItem(*this);
}

LedgerTransactionV2::LedgerTransactionV2(
      Date const& date, std::string const& filename, size_t linenum) :
   LedgerItem(filename, linenum),
   m_date(date)
{
}

Currency LedgerTransactionV2::amount() const
{
   return m_amount;
}

Date LedgerTransactionV2::date() const
{
   return m_date;
}

void LedgerTransactionV2::setDate(Date const& date)
{
   m_date = date;
}

pair<string, bool> LedgerTransactionV2::note() const
{
   return m_note;
}

void LedgerTransactionV2::setNote(string const& note)
{
   m_note.first = note;
   m_note.second = true;
}

string LedgerTransactionV2::payee() const
{
   return m_payee;
}

void LedgerTransactionV2::setPayee(string const& payee)
{
   m_payee = payee;
}

LedgerTransactionV2::Status LedgerTransactionV2::status() const
{
   return m_status;
}

void LedgerTransactionV2::setStatus(Status status)
{
   m_status = status;
}

void LedgerTransactionV2::appendEntry(
      shared_ptr<LedgerTransactionV2Entry> entry)
{
   if (!entry->amount().second)
   {
      for (auto existingEntry : m_entries)
      {
         if (!existingEntry->amount().second)
         {
            die(fileName(), lineNum(),
                "Cannot elide amounts on multiple transaction lines");
         }
      }
   }
   m_entries.push_back(entry);
}

void LedgerTransactionV2::finalize()
{
   Currency side1, side2;
   for (auto entry : m_entries)
   {
      entry->balance(side1, side2);
   }
   for (auto entry : m_entries)
   {
      entry->setBalance(side1, side2);
   }
   if (side1 != side2)
   {
      warn(fileName(), lineNum(),
           "Transaction must balance between accounts and categories");
   }
   m_amount = side1;
}

void LedgerTransactionV2::processItem(ItemProcessor& processor) const
{
   if (processor.processItem(*this))
   {
      for (auto entry : m_entries)
      {
         entry->processItem(processor);
      }
   }
}
