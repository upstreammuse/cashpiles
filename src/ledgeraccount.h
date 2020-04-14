#ifndef LEDGERACCOUNT_H
#define LEDGERACCOUNT_H

#include "currency.h"
#include "date.h"
#include "identifier.h"
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
   LedgerAccount(std::string const& filename, size_t lineNum);

   Date date() const;
   void setDate(Date const& date);

   Mode mode() const;
   void setMode(Mode mode);

   Identifier name() const;
   void setName(Identifier const& name);

   void processItem(ItemProcessor& processor) const;

private:
   Date m_date;
   Mode m_mode = Mode::CLOSED;
   Identifier m_name;
};

#endif
