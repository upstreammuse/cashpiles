#include "ledgeraccount.h"

#include <cassert>
#include "itemprocessor.h"

LedgerAccount::Mode LedgerAccount::modeFromString(std::string const& mode,
                                                  bool* ok)
{
   bool dummy;
   bool* success = ok ? ok : &dummy;
   *success = true;

   if (mode == "on-budget")
   {
      return Mode::ON_BUDGET;
   }
   else if (mode == "off-budget")
   {
      return Mode::OFF_BUDGET;
   }
   else if (mode == "close")
   {
      return Mode::CLOSED;
   }
   else
   {
      *success = false;
      return Mode::CLOSED;
   }
}

std::string LedgerAccount::modeToString(Mode mode)
{
   switch (mode)
   {
      case Mode::ON_BUDGET:
         return "on-budget";
      case Mode::OFF_BUDGET:
         return "off-budget";
      case Mode::CLOSED:
         return "close";
   }

   assert("Mode not handled" && false);
   return "";
}

LedgerAccount::LedgerAccount(std::string const& filename, size_t lineNum) :
   LedgerItem(filename, lineNum)
{
}

Date LedgerAccount::date() const
{
   assert(m_date.isValid());
   return m_date;
}

void LedgerAccount::setDate(Date const& date)
{
   assert(date.isValid());
   m_date = date;
}

LedgerAccount::Mode LedgerAccount::mode() const
{
   return m_mode;
}

void LedgerAccount::setMode(Mode mode)
{
   m_mode = mode;
}

std::string LedgerAccount::name() const
{
   return m_name;
}

void LedgerAccount::setName(std::string const& name)
{
   m_name = name;
}

void LedgerAccount::processItem(ItemProcessor& processor) const
{
   processor.processItem(*this);
}
