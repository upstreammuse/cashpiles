#include "ledgeraccount.h"

#include "itemprocessor.h"

LedgerAccount::Mode LedgerAccount::modeFromString(QString const& mode, bool* ok)
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

QString LedgerAccount::modeToString(Mode mode)
{
   switch (mode)
   {
      case Mode::ON_BUDGET:
         return "on-budget";
      case Mode::OFF_BUDGET:
         return "off-budget";
      case Mode::CLOSED:
         return "closed";
   }

   Q_ASSERT_X(false, "modeToString", "Mode not handled");
   return "";
}

LedgerAccount::LedgerAccount(QString const& filename, uint lineNum) :
   LedgerItem(filename, lineNum)
{
}

QDate LedgerAccount::date() const
{
   return m_date;
}

LedgerAccount::Mode LedgerAccount::mode() const
{
   return m_mode;
}

QString LedgerAccount::name() const
{
   return m_name;
}

void LedgerAccount::processItem(ItemProcessor& processor) const
{
   processor.processItem(*this);
}

void LedgerAccount::setDate(QDate const& date)
{
   m_date = date;
}

void LedgerAccount::setMode(Mode mode)
{
   m_mode = mode;
}

void LedgerAccount::setName(QString const& name)
{
   m_name = name;
}
