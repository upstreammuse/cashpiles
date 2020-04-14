#include "filewriter.h"

#include <sstream>
#include "cashpiles.h"
#include "ledgeraccount.h"
#include "ledgerbudget.h"
#include "ledgerbudgetcloseentry.h"
#include "ledgerbudgetgoalentry.h"
#include "ledgerbudgetincomeentry.h"
#include "ledgerbudgetreserveamountentry.h"
#include "ledgerbudgetreservepercententry.h"
#include "ledgerbudgetroutineentry.h"
#include "ledgerbudgetwithholdingentry.h"
#include "ledgercomment.h"
#include "ledgerreserve.h"
#include "ledgertransaction.h"

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

void FileWriter::processItem(LedgerBlank const&)
{
   m_file << std::endl;
}

void FileWriter::processItem(LedgerBudget const& budget)
{
   m_file << budget.date().toString(m_dateFormat) << " budget "
          << budget.interval().toString() << std::endl;
}

void FileWriter::processItem(LedgerBudgetCloseEntry const& entry)
{
   m_file << "  close   " << entry.category() << std::endl;
}

void FileWriter::processItem(LedgerBudgetGoalEntry const& entry)
{
   m_file << "  goal    " << entry.category();
   if (entry.owner().isNotEmpty())
   {
      m_file << "  " << entry.owner();
   }
   m_file << std::endl;
}

void FileWriter::processItem(LedgerBudgetIncomeEntry const& entry)
{
   m_file << "  income  " << entry.category();
   if (entry.owner().isNotEmpty())
   {
      m_file << "  " << entry.owner();
   }
   m_file << std::endl;
}

void FileWriter::processItem(LedgerBudgetReserveAmountEntry const& entry)
{
   m_file << "  reserve " << entry.category() << "  "
          << entry.amount().toString() << " " << entry.interval().toString();
   if (entry.owner().isNotEmpty())
   {
      m_file << "  " << entry.owner();
   }
   m_file << std::endl;
}

void FileWriter::processItem(LedgerBudgetReservePercentEntry const& entry)
{
   m_file << "  reserve " << entry.category() << "  " << entry.percentage()
          << "%";
   if (entry.owner().isNotEmpty())
   {
      m_file << "  " << entry.owner();
   }
   m_file << std::endl;
}

void FileWriter::processItem(LedgerBudgetRoutineEntry const& entry)
{
   m_file << "  routine " << entry.category();
   if (entry.owner().isNotEmpty())
   {
      m_file << "  " << entry.owner();
   }
   m_file << std::endl;
}

void FileWriter::processItem(LedgerBudgetWithholdingEntry const& entry)
{
   m_file << "  withholding " << entry.category();
   if (entry.owner().isNotEmpty())
   {
      m_file << "  " << entry.owner();
   }
   m_file << std::endl;
}

void FileWriter::processItem(LedgerComment const& comment)
{
   m_file << ";" << comment.note() << std::endl;
}

void FileWriter::processItem(LedgerReserve const& reserve)
{
   m_file << reserve.date().toString(m_dateFormat) << " reserve";
   if (reserve.numEntries() > 1)
   {
      m_singleReserve = false;
      m_file << std::endl;
   }
   else
   {
      m_singleReserve = true;
   }
}

void FileWriter::processItem(LedgerReserveEntry const& reserve)
{
   if (m_singleReserve)
   {
      m_file << " ";
      m_singleReserve = false;
   }
   else
   {
      m_file << "  ";
   }
   if (reserve.category().type() == Identifier::Type::OWNER)
   {
      m_file << "@";
   }
   m_file << reserve.category() << "  " << reserve.amount().toString()
          << std::endl;
}

void FileWriter::processItem(LedgerTransaction const& transaction)
{
   m_file << transaction.date().toString(m_dateFormat);
   m_file << " " << transaction.statusToString(transaction.status());
   m_file << " " << transaction.account() << "  ";
   size_t numNotes = transaction.hasNote() ? 1 : 0;
   for (LedgerTransactionEntry const& entry : transaction.entries())
   {
      if (entry.hasNote())
      {
         numNotes++;
      }
   }
   if (transaction.entries().size() > 1 || numNotes > 1)
   {
      m_file << transaction.amount().toString();
      if (transaction.hasBalance())
      {
         m_file << " = " << transaction.balance().toString();
      }
      if (transaction.hasNote())
      {
         m_file << " ;" << transaction.note();
      }
      m_file << std::endl;
      for (LedgerTransactionEntry const& entry : transaction.entries())
      {
         m_file << "  ";
         switch (entry.payee().type())
         {
            case Identifier::Type::GENERIC:
               break;
            case Identifier::Type::ACCOUNT:
               m_file << "@";
               break;
            case Identifier::Type::OWNER:
            case Identifier::Type::CATEGORY:
            case Identifier::Type::UNINITIALIZED:
               die(transaction.fileName(), transaction.lineNum(),
                   "Internal logic error, payee has invalid type");
               // break is a warning here, die does not return
         }
         m_file << entry.payee() << "  ";
         switch (entry.category().type())
         {
            case Identifier::Type::OWNER:
               m_file << "@" << entry.category() << "  ";
               break;
            case Identifier::Type::CATEGORY:
               m_file << entry.category() << "  ";
               break;
            case Identifier::Type::UNINITIALIZED:
               break;
            case Identifier::Type::GENERIC:
            case Identifier::Type::ACCOUNT:
               die(transaction.fileName(), transaction.lineNum(),
                   "Internal logic error, category has invalid type");
               // break is a warning here, die does not return
         }
         m_file << entry.amount().toString();
         if (entry.hasNote())
         {
            m_file << " ;" << entry.note();
         }
         m_file << std::endl;
      }
   }
   else
   {
      LedgerTransactionEntry entry(transaction.entries()[0]);
      switch (entry.payee().type())
      {
         case Identifier::Type::GENERIC:
            break;
         case Identifier::Type::ACCOUNT:
            m_file << "@";
            break;
         case Identifier::Type::OWNER:
         case Identifier::Type::CATEGORY:
         case Identifier::Type::UNINITIALIZED:
            die(transaction.fileName(), transaction.lineNum(),
                "Internal logic error, payee has invalid type");
            // break is a warning here because die does not return
      }
      m_file << entry.payee();
      switch (entry.category().type())
      {
         case Identifier::Type::OWNER:
            m_file << "  @" << entry.category();
            break;
         case Identifier::Type::CATEGORY:
            m_file << "  " << entry.category();
            break;
         case Identifier::Type::UNINITIALIZED:
            break;
         case Identifier::Type::GENERIC:
         case Identifier::Type::ACCOUNT:
            die(transaction.fileName(), transaction.lineNum(),
                "Internal logic error, category has invalid type");
            // break is a warning here because die does not return
      }
      m_file << "  " << transaction.amount().toString();
      if (transaction.hasBalance())
      {
         m_file << " = " << transaction.balance().toString();
      }
      if (transaction.hasNote())
      {
         m_file << " ;" << transaction.note();
      }
      else if (entry.hasNote())
      {
         m_file << " ;" << entry.note();
      }
      m_file << std::endl;
   }
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
