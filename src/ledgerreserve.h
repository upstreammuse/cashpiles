#pragma once

#include <memory>
#include <vector>
#include "currency.h"
#include "date.h"
#include "identifier.h"
#include "ledgeritem.h"

class LedgerReserveEntry : public LedgerItem
{
public:
   LedgerReserveEntry(std::string const& filename, size_t linenum);

   Currency amount() const;
   void setAmount(Currency const& amount);

   Identifier category() const;
   void setCategory(Identifier const& category);

   Date date() const;
   void setDate(Date const& date);

   void processItem(ItemProcessor& processor) const;

private:
   Currency m_amount;
   Identifier m_category;
   Date m_date;
};

class LedgerReserve : public LedgerItem
{
public:
   LedgerReserve(std::string const& filename, size_t linenum);

   Currency amount() const;

   Date date() const;
   void setDate(Date const& date);

   void appendEntry(std::shared_ptr<LedgerReserveEntry> entry);
   size_t numEntries() const;

   void processItem(ItemProcessor& processor) const;

private:
   Date m_date;
   std::vector<std::shared_ptr<LedgerReserveEntry>> m_entries;
};
