#include "ledgertransaction.h"

#include "itemprocessor.h"

LedgerTransaction::Status LedgerTransaction::statusFromString(
      std::string const& status, bool* ok)
{
   bool dummy;
   bool* success = ok ? ok : &dummy;
   *success = true;

   if (status == "*")
   {
      return Status::CLEARED;
   }
   else if (status == "?")
   {
      return Status::PENDING;
   }
   else if (status == "!")
   {
      return Status::DISPUTED;
   }
   else
   {
      *success = false;
      return Status::PENDING;
   }

}

QString LedgerTransaction::statusToString(Status status)
{
   switch (status)
   {
      case Status::CLEARED:
         return "*";
      case Status::PENDING:
         return "?";
      case Status::DISPUTED:
         return "!";
   }

   Q_ASSERT_X(false, "statusToString", "Status not handled");
   return "";
}

LedgerTransaction::LedgerTransaction(QString const& filename, uint lineNum) :
   LedgerItem(filename, lineNum)
{
}

Identifier LedgerTransaction::account() const
{
   return m_account;
}

void LedgerTransaction::setAccount(Identifier const& account)
{
   m_account = account;
}

Currency LedgerTransaction::amount() const
{
   Currency amount;
   foreach (LedgerTransactionEntry const& entry, m_entries)
   {
      amount += entry.amount();
   }
   return amount;
}

Currency LedgerTransaction::balance() const
{
   Q_ASSERT(m_balance);
   return *m_balance;
}

bool LedgerTransaction::hasBalance() const
{
   return m_balance;
}

void LedgerTransaction::setBalance(Currency const& balance)
{
   m_balance.reset(new Currency(balance));
}

QDate LedgerTransaction::date() const
{
   return m_date;
}

void LedgerTransaction::setDate(QDate const& date)
{
   m_date = date;
}

QString LedgerTransaction::note() const
{
   Q_ASSERT(m_note);
   return *m_note;
}

bool LedgerTransaction::hasNote() const
{
   return m_note;
}

void LedgerTransaction::setNote(std::string const& note)
{
   m_note.reset(new QString(QString::fromStdString(note)));
}

LedgerTransaction::Status LedgerTransaction::status() const
{
   return m_status;
}

void LedgerTransaction::setStatus(Status status)
{
   m_status = status;
}

void LedgerTransaction::appendEntry(LedgerTransactionEntry const& entry)
{
   m_entries.append(entry);
}

QList<LedgerTransactionEntry> const& LedgerTransaction::entries() const
{
   return m_entries;
}

void LedgerTransaction::processItem(ItemProcessor& processor) const
{
   processor.processItem(*this);
}
