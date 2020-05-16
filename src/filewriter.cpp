#include "filewriter.h"

#include <sstream>
#include "cashpiles.h"
#include "ledgeraccount.h"
#include "ledgeraccountbalance.h"
#include "ledgerbudget.h"
#include "ledgerbudgetcancelentry.h"
#include "ledgerbudgetcloseentry.h"
#include "ledgerbudgetgoalentry.h"
#include "ledgerbudgetgoalsentry.h"
#include "ledgerbudgetincomeentry.h"
#include "ledgerbudgetreserveamountentry.h"
#include "ledgerbudgetreservepercententry.h"
#include "ledgerbudgetroutineentry.h"
#include "ledgerbudgetwithholdingentry.h"
#include "ledgercomment.h"
#include "ledgertransaction.h"
#include "ledgertransactionv2.h"

FileWriter::FileWriter(std::string const& fileName) :
   m_fileName(fileName)
{
}

void FileWriter::finish()
{
   if (!m_file)
   {
      std::stringstream ss;
      ss << "Unable to write to output file '" << m_fileName << "'";
      die(ss.str());
   }
   m_file.close();
}

void FileWriter::processItem(LedgerAccount const& account)
{
   m_file << account.date().toString(m_dateFormat) << " "
          << account.modeToString(account.mode()) << " " << account.name()
          << std::endl;
}

void FileWriter::processItem(LedgerAccountBalance const& balance)
{
   m_file << balance.date().toString(m_dateFormat) << " balance "
          << balance.account() << "  " << balance.amount().toString()
          << std::endl;
}

void FileWriter::processItem(LedgerBlank const&)
{
   m_file << std::endl;
}

bool FileWriter::processItem(LedgerBudget const& budget)
{
   m_file << budget.date().toString(m_dateFormat) << " budget "
          << budget.interval().toString() << std::endl;
   return true;
}

void FileWriter::processItem(LedgerBudgetCancelEntry const& entry)
{
   m_file << "  cancel  " << entry.category() << "  " << entry.goal()
          << std::endl;
}

void FileWriter::processItem(LedgerBudgetCloseEntry const& entry)
{
   m_file << "  close   " << entry.category() << std::endl;
}

void FileWriter::processItem(LedgerBudgetGoalEntry const& entry)
{
   m_file << "  goal    " << entry.category() << "  " << entry.goal() << "  "
          << entry.amount().toString() << " " << entry.goalDate().toString()
          << std::endl;
}

void FileWriter::processItem(LedgerBudgetGoalsEntry const& entry)
{
   m_file << "  goals   " << entry.category();
   if (entry.owner() != "")
   {
      m_file << "  " << entry.owner();
   }
   m_file << std::endl;
}

void FileWriter::processItem(LedgerBudgetIncomeEntry const& entry)
{
   m_file << "  income  " << entry.category();
   if (entry.owner() != "")
   {
      m_file << "  " << entry.owner();
   }
   m_file << std::endl;
}

void FileWriter::processItem(LedgerBudgetReserveAmountEntry const& entry)
{
   m_file << "  reserve " << entry.category() << "  "
          << entry.amount().toString() << " " << entry.interval().toString();
   if (entry.owner() != "")
   {
      m_file << "  " << entry.owner();
   }
   m_file << std::endl;
}

void FileWriter::processItem(LedgerBudgetReservePercentEntry const& entry)
{
   m_file << "  reserve " << entry.category() << "  " << entry.percentage()
          << "%";
   if (entry.owner() != "")
   {
      m_file << "  " << entry.owner();
   }
   m_file << std::endl;
}

void FileWriter::processItem(LedgerBudgetRoutineEntry const& entry)
{
   m_file << "  routine " << entry.category();
   if (entry.owner() != "")
   {
      m_file << "  " << entry.owner();
   }
   m_file << std::endl;
}

void FileWriter::processItem(LedgerBudgetWithholdingEntry const& entry)
{
   m_file << "  withholding " << entry.category();
   if (entry.owner() != "")
   {
      m_file << "  " << entry.owner();
   }
   m_file << std::endl;
}

void FileWriter::processItem(LedgerComment const& comment)
{
   m_file << ";" << comment.note() << std::endl;
}

void FileWriter::processItem(LedgerTransaction const& transaction)
{
   m_file << transaction.date().toString(m_dateFormat) << " ";
   switch (transaction.status())
   {
      case LedgerTransaction::Status::CLEARED:
         m_file << "*";
         break;
      case LedgerTransaction::Status::DISPUTED:
         m_file << "!";
         break;
      case LedgerTransaction::Status::PENDING:
         m_file << "?";
         break;
   }
   m_file << " " << transaction.account() << "  " << transaction.payee() << "  "
          << transaction.amount().toString();
   if (transaction.note().second)
   {
      m_file << " ;" << transaction.note().first;
   }
   m_file << std::endl;
}

bool FileWriter::processItem(LedgerTransactionV2 const& transaction)
{
   m_file << transaction.date().toString(m_dateFormat) << ' ';
   switch (transaction.status())
   {
      case LedgerTransactionV2::Status::CLEARED:
         m_file << '*';
         break;
      case LedgerTransactionV2::Status::DISPUTED:
         m_file << '!';
         break;
      case LedgerTransactionV2::Status::PENDING:
         m_file << '?';
         break;
   }
   m_file << ' ' << transaction.payee() << "  "
          << transaction.amount().toString();
   if (transaction.note().second)
   {
      m_file << " ;" << transaction.note().first;
   }
   m_file << std::endl;
   return true;
}

void FileWriter::processItem(LedgerTransactionV2AccountEntry const& entry)
{
   m_file << "  " << entry.account() << "  " << entry.amount().first.toString();
   if (entry.note().second)
   {
      m_file << " ;" << entry.note().first;
   }
   m_file << std::endl;
}

void FileWriter::processItem(LedgerTransactionV2CategoryEntry const& entry)
{
   m_file << "  " << entry.category();
   if (entry.trackingAccount().second)
   {
      m_file << "  " << entry.trackingAccount().first;
   }
   m_file << "  " << entry.amount().first.toString();
   if (entry.note().second)
   {
      m_file << " ;" << entry.note().first;
   }
   m_file << std::endl;
}

void FileWriter::processItem(LedgerTransactionV2OwnerEntry const& entry)
{
   m_file << "  " << entry.owner();
   if (entry.trackingAccount().second)
   {
      m_file << "  " << entry.trackingAccount().first;
   }
   m_file << "  " << entry.amount().first.toString();
   if (entry.note().second)
   {
      m_file << " ;" << entry.note().first;
   }
   m_file << std::endl;
}

void FileWriter::setDateFormat(std::string const& dateFormat)
{
   m_dateFormat = dateFormat;
}

void FileWriter::start()
{
   m_file.open(m_fileName);
   if (!m_file)
   {
      std::stringstream ss;
      ss << "Unable to open output file '" << m_fileName << "'";
      die(ss.str());
   }
}
