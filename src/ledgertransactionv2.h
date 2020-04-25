#pragma once

#include "ledgeritem.h"

class Currency;
class Date;
class Identifier;

class LedgerTransactionV2Entry : public LedgerItem
{
public:
   std::pair<Currency, bool> amount() const;
   std::pair<std::string, bool> note() const;
};

class LedgerTransactionV2AccountEntry : public LedgerTransactionV2Entry
{
public:
   Identifier account() const;
};

class LedgerTransactionV2CategoryEntry : public LedgerTransactionV2Entry
{
public:
   Identifier category() const;
   std::pair<Identifier, bool> trackingAccount() const;
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
   Currency amount() const;

   Date date() const;
   void setDate(Date const& date);

   std::pair<std::string, bool> note() const;
   void setNote(std::string const& note);

   Identifier payee() const;
   void setPayee(Identifier const& payee);

   Status status() const;
   void setStatus(Status status);

   void appendEntry(std::shared_ptr<LedgerTransactionV2Entry> entry);
   void processItem(ItemProcessor& processor) const;
};
