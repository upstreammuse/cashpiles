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
         return "close";
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
   Q_ASSERT(m_date.isValid());
   return m_date;
}

void LedgerAccount::setDate(QDate const& date)
{
   Q_ASSERT(date.isValid());
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

Identifier LedgerAccount::name() const
{
   Q_ASSERT(m_name.type() == Identifier::Type::ACCOUNT);
   return m_name;
}

void LedgerAccount::setName(Identifier const& name)
{
   Q_ASSERT(name.type() == Identifier::Type::ACCOUNT);
   m_name = name;
}

void LedgerAccount::processItem(ItemProcessor& processor) const
{
   processor.processItem(*this);
}
