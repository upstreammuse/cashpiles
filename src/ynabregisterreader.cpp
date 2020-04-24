#include "ynabregisterreader.h"

#include "cashpiles.h"
#include "csvreader.h"
#include "date.h"
#include "filereader.h"
#include "ledger.h"
#include "ledgererror.h"
#include "ledgertransaction.h"

YnabRegisterReader::YnabRegisterReader(std::string const& fileName,
                                       Ledger& ledger) :
   m_fileName(fileName),
   m_ledger(ledger),
   m_reader(fileName)
{
}

void YnabRegisterReader::readAll()
{
   m_ledger.clear();
   if (!m_reader.openFile())
   {
      std::stringstream ss;
      ss << "Unable to open input file '" << m_fileName << "'";
      auto error = std::make_shared<LedgerError>();
      error->insertMessage(ss.str(), {m_fileName, 0, 0});
      m_ledger.appendItem(error);
   }
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

void YnabRegisterReader::processRecord(CsvReader::Record const& record)
{
   if (!m_transaction)
   {
      m_transaction.reset(
               new LedgerTransaction(record.fileName, record.lineNum));
   }
   bool inSplit = false;

   LedgerTransactionEntry entry;
   Currency inflow;
   Currency outflow;
   std::string note;
   for (auto it : record.data)
   {
      if (it.first == "Account")
      {
         m_transaction->setAccount(
                  Identifier(it.second, Identifier::Type::ACCOUNT));
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
            entry.setCategory(
                     Identifier(it.second, Identifier::Type::CATEGORY));
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
            m_transaction->setStatus(LedgerTransaction::Status::PENDING);
         }
         else if (it.second == "C" || it.second == "Cleared" || it.second == "Reconciled")
         {
            m_transaction->setStatus(LedgerTransaction::Status::CLEARED);
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
                  FileReader::parseDate(it.second, m_dateFormat, record.fileName,
                                        record.lineNum));
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
         inflow = Currency::fromString(it.second);
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
         else if (std::regex_match(it.second, match, splitV4Rx))
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
         outflow = Currency::fromString(it.second);
      }
      else if (it.first == "Payee")
      {
         std::regex const transferRx("^Transfer : (.*)$");
         std::smatch match;
         if (std::regex_match(it.second, match, transferRx))
         {
            entry.setPayee(
                     Identifier(match.str(1), Identifier::Type::ACCOUNT));
         }
         else
         {
            entry.setPayee(Identifier(it.second, Identifier::Type::GENERIC));
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
   entry.setAmount(inflow - outflow);
   if (note != "")
   {
      entry.setNote(note);
   }
   m_transaction->appendEntry(entry);

   if (!inSplit)
   {
      m_items.insert(std::make_pair(m_transaction->date(), m_transaction));
      m_transaction.reset();
   }
}
