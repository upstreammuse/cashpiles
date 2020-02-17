#include "ynabregisterreader.h"

#include "cashpiles.h"
#include "csvreader.h"
#include "filereader.h"
#include "ledger.h"
#include "ledgertransaction.h"

YnabRegisterReader::YnabRegisterReader(QString const& fileName, Ledger& ledger,
                                       QObject* parent) :
   QObject(parent),
   m_ledger(ledger),
   m_reader(new CsvReader(fileName, this))
{
}

void YnabRegisterReader::readAll()
{
   m_reader->openFile();
   while (m_reader->hasContent())
   {
      CsvReader::Record record(m_reader->readRecord());
      processRecord(record);
   }
   m_reader->closeFile();

   if (!m_transaction.isNull())
   {
      warn("YNAB file had extra record data at end");
   }
   foreach (QSharedPointer<LedgerItem> item, m_items)
   {
      m_ledger.appendItem(item);
   }
}

void YnabRegisterReader::setDateFormat(QString const& dateFormat)
{
   m_dateFormat = dateFormat;
}

void YnabRegisterReader::processRecord(CsvReader::Record const& record)
{
   if (m_transaction.isNull())
   {
      m_transaction.reset(
               new LedgerTransaction(record.fileName, record.lineNum));
   }
   bool inSplit = false;

   LedgerTransactionEntry entry;
   Currency inflow;
   Currency outflow;
   QString note;
   for (QHash<QString, QString>::const_iterator it(record.data.cbegin());
        it != record.data.cend(); ++it)
   {
      if (it.key() == "Account")
      {
         m_transaction->setAccount(Identifier(*it, Identifier::Type::ACCOUNT));
      }
      else if (it.key() == "Category" || it.key() == "Category Group" ||
               it.key() == "Master Category" || it.key() == "Sub Category")
      {
         // ignored, part of full category name we use
      }
      else if (it.key() == "Category Group/Category")
      {
         if (*it != "")
         {
            entry.setCategory(Identifier(*it, Identifier::Type::CATEGORY));
         }
      }
      else if (it.key() == "Check Number")
      {
         if (*it != "" )
         {
            note += QString("[checkNumber=%1]").arg(*it);
         }
      }
      else if (it.key() == "Cleared")
      {
         if (*it == "U" || *it == "Uncleared")
         {
            m_transaction->setStatus(LedgerTransaction::Status::PENDING);
         }
         else if (*it == "C" || *it == "Cleared" || *it == "Reconciled")
         {
            m_transaction->setStatus(LedgerTransaction::Status::CLEARED);
         }
         else
         {
            die(record.fileName, record.lineNum,
                QString("Unknown cleared status '%1'")
                .arg(*it));
         }
      }
      else if (it.key() == "Date")
      {
         m_transaction->setDate(
                  FileReader::parseDate(*it, m_dateFormat, record.fileName,
                                        record.lineNum));
      }
      else if (it.key() == "Flag")
      {
         if (*it != "")
         {
            note += QString("[flag=%1]").arg(*it);
         }
      }
      else if (it.key() == "Inflow")
      {
         inflow = FileReader::parseCurrency(*it, record.fileName,
                                            record.lineNum);
      }
      else if (it.key() == "Memo")
      {
         QRegularExpression const splitV4Rx(
                  "^\\(Split ([0-9]*)/([0-9]*)\\) (.*)$");
         QRegularExpression const splitV5Rx(
                  "^Split \\(([0-9]*)/([0-9]*)\\) (.*)$");
         QRegularExpressionMatch match;
         QString memo;
         if ((match = splitV4Rx.match(*it)).hasMatch())
         {
            memo = match.captured(3);
            inSplit = match.captured(1).toInt() != match.captured(2).toInt();
         }
         else if ((match = splitV5Rx.match(*it)).hasMatch())
         {
            memo = match.captured(3);
            inSplit = match.captured(1).toInt() != match.captured(2).toInt();
         }
         else
         {
            memo = *it;
         }
         if (memo != "")
         {
            note += QString("[memo=%1]").arg(memo);
         }
      }
      else if (it.key() == "Outflow")
      {
         outflow = FileReader::parseCurrency(*it, record.fileName, record.lineNum);
      }
      else if (it.key() == "Payee")
      {
         QRegularExpression const transferRx("^Transfer : (.*)$");
         QRegularExpressionMatch match = transferRx.match(*it);
         if (match.hasMatch())
         {
            entry.setPayee(
                     Identifier(match.captured(1), Identifier::Type::ACCOUNT));
         }
         else
         {
            entry.setPayee(Identifier(*it, Identifier::Type::GENERIC));
         }
      }
      else if (it.key() == "Running Balance")
      {
         // ignored, no CP equivalent
      }
      else
      {
         die(QString("YNAB file has unknown column header '%1'")
             .arg(it.key()));
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
      m_items.insertMulti(m_transaction->date(), m_transaction);
      m_transaction.reset();
   }
}
