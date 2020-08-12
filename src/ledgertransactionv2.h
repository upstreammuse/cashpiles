#pragma once

#include <memory>
#include <vector>
#include "currency.h"
#include "date.h"
#include "ledgeritem.h"

class LedgerTransactionV2Entry : public LedgerItem
{
public:
   using LedgerItem::LedgerItem;

   std::pair<Currency, bool> amount() const;
   void setAmount(Currency const& amount);

   virtual void balance(Currency& accounts, Currency& categories) const = 0;
   virtual void setBalance(Currency& accounts, Currency& categories) = 0;

   std::pair<std::string, bool> note() const;
   void setNote(std::string const& note);

private:
   std::pair<Currency, bool> m_amount = std::make_pair(Currency{}, false);
   std::pair<std::string, bool> m_note = std::make_pair("", false);
};

class LedgerTransactionV2AccountEntry : public LedgerTransactionV2Entry
{
public:
   using LedgerTransactionV2Entry::LedgerTransactionV2Entry;

   std::string account() const;
   void setAccount(std::string const& account);

   void balance(Currency& accounts, Currency& categories) const;
   void setBalance(Currency& accounts, Currency& categories);

   void processItem(ItemProcessor& processor) const;

private:
   std::string m_account;
};

class LedgerTransactionV2CategoryEntry : public LedgerTransactionV2Entry
{
public:
   using LedgerTransactionV2Entry::LedgerTransactionV2Entry;

   void balance(Currency& accounts, Currency& categories) const;
   void setBalance(Currency& accounts, Currency& categories);

   std::string category() const;
   void setCategory(std::string const& category);

   std::pair<std::string, bool> trackingAccount() const;
   void setTrackingAccount(std::string const& account);

   void processItem(ItemProcessor& processor) const;

private:
   std::string m_category;
   std::pair<std::string, bool> m_trackingAccount = std::make_pair("", false);
};

class LedgerTransactionV2OwnerEntry : public LedgerTransactionV2Entry
{
public:
   using LedgerTransactionV2Entry::LedgerTransactionV2Entry;

   void balance(Currency& accounts, Currency& categories) const;
   void setBalance(Currency& accounts, Currency& categories);

   std::string owner() const;
   void setOwner(std::string const& owner);

   std::pair<std::string, bool> trackingAccount() const;
   void setTrackingAccount(std::string const& account);

   void processItem(ItemProcessor& processor) const;

private:
   std::string m_owner;
   std::pair<std::string, bool> m_trackingAccount = std::make_pair("", false);
};

class LedgerTransactionV2 : public LedgerItem
{
public:
   enum class Status
   {
      CLEARED,
      DISPUTED,
      PENDING
   };

public:
   LedgerTransactionV2(
         Date const& date, std::string const& filename, size_t linenum);

   Currency amount() const;

   Date date() const;
   void setDate(Date const& date);

   std::pair<std::string, bool> note() const;
   void setNote(std::string const& note);

   std::string payee() const;
   void setPayee(std::string const& payee);

   Status status() const;
   void setStatus(Status status);

   void appendEntry(std::shared_ptr<LedgerTransactionV2Entry> entry);
   void finalize();
   void processItem(ItemProcessor& processor) const;

private:
   Currency m_amount;
   Date m_date;
   std::vector<std::shared_ptr<LedgerTransactionV2Entry>> m_entries;
   std::pair<std::string, bool> m_note = std::make_pair("", false);
   std::string m_payee;
   Status m_status = Status::PENDING;
};
