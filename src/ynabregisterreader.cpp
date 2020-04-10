#include "ynabregisterreader.h"

#include <QRegularExpression>
#include "cashpiles.h"
#include "csvreader.h"
#include "date.h"
#include "filereader.h"
#include "ledger.h"
#include "ledgertransaction.h"

YnabRegisterReader::YnabRegisterReader(QString const& fileName, Ledger& ledger,
                                       QObject* parent) :
   QObject(parent),
   m_ledger(ledger),
   m_reader(fileName.toStdString())
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
      m_ledger.appendItem(it);
   }
}

void YnabRegisterReader::setDateFormat(QString const& dateFormat)
{
   m_dateFormat = dateFormat.toStdString();
}

void YnabRegisterReader::processRecord(CsvReader::Record const& record)
{
   if (!m_transaction)
   {
      m_transaction.reset(
               new LedgerTransaction(QString::fromStdString(record.fileName), record.lineNum));
   }
   bool inSplit = false;

   LedgerTransactionEntry entry;
   Currency inflow;
   Currency outflow;
   QString note;
   for (auto it : record.data)
   {
      if (it.first == "Account")
      {
         m_transaction->setAccount(
                  Identifier(QString::fromStdString(it.second), Identifier::Type::ACCOUNT));
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
            entry.setCategory(Identifier(QString::fromStdString(it.second), Identifier::Type::CATEGORY));
         }
      }
      else if (it.first == "Check Number")
      {
         if (it.second != "" )
         {
            note += QString("[checkNumber=%1]").arg(QString::fromStdString(it.second));
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
            die(record.fileName, record.lineNum,
                QString("Unknown cleared status '%1'")
                .arg(QString::fromStdString(it.second)).toStdString());
         }
      }
      else if (it.first == "Date")
      {
         m_transaction->setDate(
                  FileReader::parseDate(it.second, m_dateFormat, record.fileName,
                                        record.lineNum).toQDate());
      }
      else if (it.first == "Flag")
      {
         if (it.second != "")
         {
            note += QString("[flag=%1]").arg(QString::fromStdString(it.second));
         }
      }
      else if (it.first == "Inflow")
      {
         inflow = FileReader::parseCurrency(it.second, record.fileName,
                                            record.lineNum);
      }
      else if (it.first == "Memo")
      {
         QRegularExpression const splitV4Rx(
                  "^\\(Split ([0-9]*)/([0-9]*)\\) (.*)$");
         QRegularExpression const splitV5Rx(
                  "^Split \\(([0-9]*)/([0-9]*)\\) (.*)$");
         QRegularExpressionMatch match;
         QString memo;
         if ((match = splitV4Rx.match(QString::fromStdString(it.second))).hasMatch())
         {
            memo = match.captured(3);
            inSplit = match.captured(1).toInt() != match.captured(2).toInt();
         }
         else if ((match = splitV5Rx.match(QString::fromStdString(it.second))).hasMatch())
         {
            memo = match.captured(3);
            inSplit = match.captured(1).toInt() != match.captured(2).toInt();
         }
         else
         {
            memo = QString::fromStdString(it.second);
         }
         if (memo != "")
         {
            note += QString("[memo=%1]").arg(memo);
         }
      }
      else if (it.first == "Outflow")
      {
         outflow = FileReader::parseCurrency(it.second, record.fileName, record.lineNum);
      }
      else if (it.first == "Payee")
      {
         QRegularExpression const transferRx("^Transfer : (.*)$");
         QRegularExpressionMatch match = transferRx.match(QString::fromStdString(it.second));
         if (match.hasMatch())
         {
            entry.setPayee(
                     Identifier(match.captured(1), Identifier::Type::ACCOUNT));
         }
         else
         {
            entry.setPayee(Identifier(QString::fromStdString(it.second), Identifier::Type::GENERIC));
         }
      }
      else if (it.first == "Running Balance")
      {
         // ignored, no CP equivalent
      }
      else
      {
         die(QString("YNAB file has unknown column header '%1'")
             .arg(QString::fromStdString(it.first)).toStdString());
      }
   }
   entry.setAmount(inflow - outflow);
   if (note != "")
   {
      entry.setNote(note.toStdString());
   }
   m_transaction->appendEntry(entry);

   if (!inSplit)
   {
      m_items.insertMulti(m_transaction->date(), m_transaction);
      m_transaction.reset();
   }
}
