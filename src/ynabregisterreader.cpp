#include "ynabregisterreader.h"

#include <sstream>
#include "cashpiles.h"
#include "csvreader.h"
#include "date.h"
#include "filereader.h"
#include "ledger.h"
#include "ledgertransaction.h"
#include "ledgertransactionv2.h"

YnabRegisterReader::YnabRegisterReader(std::string const& fileName,
                                       Ledger& ledger) :
   m_ledger(ledger),
   m_reader(fileName)
{
}

void YnabRegisterReader::readAll()
{
   m_reader.openFile();
   while (m_reader.hasContent())
   {
      CsvReader::Record record(m_reader.readRecord());
      processRecord(record);
   }
   m_reader.closeFile();

   if (m_transaction)
   {
      warn("YNAB file had extra record data at end");
   }
   for (auto it : m_items)
   {
      m_ledger.appendItem(it.second);
   }
}

void YnabRegisterReader::setDateFormat(std::string const& dateFormat)
{
   m_dateFormat = dateFormat;
}

Currency YnabRegisterReader::parseCurrency(
      std::string const& currency, std::string const& filename, size_t linenum)
{
   bool ok;
   Currency c(Currency::fromString(currency, &ok));
   if (!ok)
   {
      std::stringstream ss;
      ss << "Unable to parse currency '" << currency << "'";
      die(filename, linenum, ss.str());
   }
   return c;
}

Date YnabRegisterReader::parseDate(
      std::string const& date, std::string const& filename, size_t linenum)
{
   try
   {
      return Date::fromString(date, m_dateFormat);
   }
   catch (...)
   {
      std::stringstream ss;
      ss << "Unable to parse date '" << date << "', expected something like '"
         << m_dateFormat << "'";
      die(filename, linenum, ss.str());
   }
}

void YnabRegisterReader::processRecord(CsvReader::Record const& record)
{
   bool addAccountLine = false;
   bool inSplit = false;
   if (!m_transaction)
   {
      m_transaction = std::make_shared<LedgerTransactionV2>(
                         DateBuilder().month(1).day(1).year(1).toDate(),
                         record.fileName, record.lineNum);
      addAccountLine = true;
   }

   std::string account;
   std::string category;
   Currency inflow;
   std::string note;
   Currency outflow;
   std::string trackingAccount;

   for (auto it : record.data)
   {
      if (it.first == "Account")
      {
         account = it.second;
      }
      else if (it.first == "Category" || it.first == "Category Group" ||
               it.first == "Master Category" || it.first == "Sub Category")
      {
         // ignored, part of full category name we use
      }
      else if (it.first == "Category Group/Category")
      {
         if (it.second != "")
         {
            category = it.second;
         }
      }
      else if (it.first == "Check Number")
      {
         if (it.second != "" )
         {
            std::stringstream ss;
            ss << "[checkNumber=" << it.second << "]";
            note += ss.str();
         }
      }
      else if (it.first == "Cleared")
      {
         if (it.second == "U" || it.second == "Uncleared")
         {
            m_transaction->setStatus(LedgerTransactionV2::Status::PENDING);
         }
         else if (it.second == "C" || it.second == "Cleared" ||
                  it.second == "Reconciled")
         {
            m_transaction->setStatus(LedgerTransactionV2::Status::CLEARED);
         }
         else
         {
            std::stringstream ss;
            ss << "Unknown cleared status '" << it.second << "'";
            die(record.fileName, record.lineNum, ss.str());
         }
      }
      else if (it.first == "Date")
      {
         m_transaction->setDate(
                  parseDate(it.second, record.fileName, record.lineNum));
      }
      else if (it.first == "Flag")
      {
         if (it.second != "")
         {
            std::stringstream ss;
            ss << "[flag=" << it.second << "]";
            note += ss.str();
         }
      }
      else if (it.first == "Inflow")
      {
         inflow = parseCurrency(it.second, record.fileName, record.lineNum);
      }
      else if (it.first == "Memo")
      {
         std::regex const splitV4Rx("^\\(Split ([0-9]*)/([0-9]*)\\) (.*)$");
         std::regex const splitV5Rx("^Split \\(([0-9]*)/([0-9]*)\\) (.*)$");
         std::smatch match;
         std::string memo;
         if (std::regex_match(it.second, match, splitV4Rx))
         {
            memo = match.str(3);
            inSplit = match.str(1) != match.str(2);
         }
         else if (std::regex_match(it.second, match, splitV5Rx))
         {
            memo = match.str(3);
            inSplit = match.str(1) != match.str(2);
         }
         else
         {
            memo = it.second;
         }
         if (memo != "")
         {
            std::stringstream ss;
            ss << "[memo=" << memo << "]";
            note += ss.str();
         }
      }
      else if (it.first == "Outflow")
      {
         outflow = parseCurrency(it.second, record.fileName, record.lineNum);
      }
      else if (it.first == "Payee")
      {
         std::regex const transferRx("^Transfer : (.*)$");
         std::smatch match;
         if (std::regex_match(it.second, match, transferRx))
         {
            trackingAccount = match[1];
         }
         else if (it.second != "")
         {
            if (m_transaction->payee() == "")
            {
               m_transaction->setPayee(it.second);
            }
            else if (m_transaction->payee() != it.second)
            {
               std::stringstream ss;
               ss << "[payee=" << it.second << "]";
               note += ss.str();
            }
         }
      }
      else if (it.first == "Running Balance")
      {
         // ignored, no CP equivalent
      }
      else
      {
         std::stringstream ss;
         ss << "YNAB file has unknown column header '" << it.first << "'";
         die(ss.str());
      }
   }

   // if this is the first line of the transaction, add a line for the YNAB primary transaction account
   if (addAccountLine)
   {
      auto entry = std::make_shared<LedgerTransactionV2AccountEntry>(record.fileName, record.lineNum);
      entry->setAccount(account);
      m_transaction->appendEntry(entry);
   }

   // expense from on-budget (category) -or-
   // transfer from on-budget to off-budget (category)
   if (category != "")
   {
      auto entry = std::make_shared<LedgerTransactionV2CategoryEntry>(record.fileName, record.lineNum);
      entry->setAmount(inflow - outflow);
      entry->setCategory(category);
      if (note != "")
      {
         entry->setNote(note);
      }
      if (trackingAccount != "")
      {
         entry->setTrackingAccount(trackingAccount);
      }
      m_transaction->appendEntry(entry);
   }
   // transfer from off-budget (no category) -or-
   // transfer from on-budget to on-budget (no category)  <- assuming this
   else if (trackingAccount != "")
   {
      auto entry = std::make_shared<LedgerTransactionV2AccountEntry>(record.fileName, record.lineNum);
      entry->setAccount(trackingAccount);
      // invert amount because we are entering the other side of the transfer
      entry->setAmount(outflow - inflow);
      note = "WARNING this transfer will be duplicated in another transaction " + note;
      entry->setNote(note);
      m_transaction->appendEntry(entry);
   }
   // expense from off-budget (no category)
   else
   {
      auto txnOB = std::make_shared<LedgerTransaction>(
                      m_transaction->date(), record.fileName, record.lineNum);
      if (note != "")
      {
         txnOB->setNote(note);
      }
      txnOB->setPayee(m_transaction->payee());
      txnOB->setAmount(inflow - outflow);
      switch (m_transaction->status())
      {
         case LedgerTransactionV2::Status::CLEARED:
            txnOB->setStatus(LedgerTransaction::Status::CLEARED);
            break;
         case LedgerTransactionV2::Status::DISPUTED:
            txnOB->setStatus(LedgerTransaction::Status::DISPUTED);
            break;
         case LedgerTransactionV2::Status::PENDING:
            txnOB->setStatus(LedgerTransaction::Status::PENDING);
            break;
      }
      txnOB->setAccount(account);
      m_items.insert(std::make_pair(txnOB->date(), txnOB));

      // TODO this is a hack to make sure we don't try to use the V2 transaction below
      inSplit = true;
      m_transaction.reset();
   }

   if (!inSplit)
   {
      m_transaction->finalize();
      m_items.insert(std::make_pair(m_transaction->date(), m_transaction));
      m_transaction.reset();
   }
}
