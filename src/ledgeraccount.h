#pragma once

#include "currency.h"
#include "date.h"
#include "ledgeritem.h"

class LedgerAccount : public LedgerItem
{
public:
   enum class Mode
   {
      ON_BUDGET,
      OFF_BUDGET,
      CLOSED
   };
   static Mode modeFromString(std::string const& mode, bool* ok);
   static std::string modeToString(Mode mode);

public:
   LedgerAccount(Date const& date, std::string const& filename, size_t lineNum);
   LedgerAccount(Date const& date, LedgerItem const&);

   Date date() const;
   void setDate(Date const& date);

   Mode mode() const;
   void setMode(Mode mode);

   std::string name() const;
   void setName(std::string const& name);

   void processItem(ItemProcessor& processor) const;

private:
   Date m_date;
   Mode m_mode = Mode::CLOSED;
   std::string m_name;
};
