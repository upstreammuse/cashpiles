#include "filewriter.h"

#include <QFile>
#include <QTextStream>
#include "cashpiles.h"
#include "ledgeraccount.h"
#include "ledgerbudget.h"
#include "ledgercomment.h"
#include "ledgerreserve.h"
#include "ledgertransaction.h"

FileWriter::FileWriter(QString const& fileName, QObject* parent) :
   QObject(parent),
   m_file(new QFile(fileName, this)),
   m_fileName(fileName)
{
}

// TODO nothing for now, but...
// fancy version should cache the input, measure the lengths of the resulting
//  strings, and then write to make everything line up nicely
void FileWriter::finish()
{
   if (m_file)
   {
      m_file->close();
   }
}

void FileWriter::processItem(LedgerAccount const& account)
{
   QTextStream out(m_file);
   out << account.date().toString(m_dateFormat) << " "
       << account.modeToString(account.mode()) << " " << account.name() << endl;
   if (out.status() != QTextStream::Status::Ok)
   {
      die(QString("Unable to write to output file").arg(m_fileName));
   }
}

void FileWriter::processItem(LedgerBlank const&)
{
   QTextStream out(m_file);
   out << endl;
   if (out.status() != QTextStream::Status::Ok)
   {
      die(QString("Unable to write to output file").arg(m_fileName));
   }
}

void FileWriter::processItem(LedgerBudget const& budget)
{
   QTextStream out(m_file);
   out << budget.date().toString(m_dateFormat) << " budget "
       << budget.interval().toString() << endl;
   if (out.status() != QTextStream::Status::Ok)
   {
      die(QString("Unable to write to output file").arg(m_fileName));
   }
}

void FileWriter::processItem(LedgerBudgetGoalEntry const& entry)
{
   QTextStream out(m_file);
   out << "  goal    " << entry.name();
   if (!entry.owner().isEmpty())
   {
      out << "  " << entry.owner();
   }
   out << endl;
   if (out.status() != QTextStream::Status::Ok)
   {
      die(QString("Unable to write to output file").arg(m_fileName));
   }
}

void FileWriter::processItem(LedgerBudgetIncomeEntry const& entry)
{
   QTextStream out(m_file);
   out << "  income  " << entry.name();
   if (!entry.owner().isEmpty())
   {
      out << "  " << entry.owner();
   }
   out << endl;
   if (out.status() != QTextStream::Status::Ok)
   {
      die(QString("Unable to write to output file").arg(m_fileName));
   }
}

void FileWriter::processItem(LedgerBudgetReserveAmountEntry const& entry)
{
   QTextStream out(m_file);
   out << "  reserve " << entry.name() << "  " << entry.amount().toString()
       << " " << entry.interval().toString();
   if (!entry.owner().isEmpty())
   {
      out << "  " << entry.owner();
   }
   out << endl;
   if (out.status() != QTextStream::Status::Ok)
   {
      die(QString("Unable to write to output file").arg(m_fileName));
   }
}

void FileWriter::processItem(LedgerBudgetReservePercentEntry const& entry)
{
   QTextStream out(m_file);
   out << "  reserve " << entry.name() << "  " << entry.percentage() << "%";
   if (!entry.owner().isEmpty())
   {
      out << "  " << entry.owner();
   }
   out << endl;
   if (out.status() != QTextStream::Status::Ok)
   {
      die(QString("Unable to write to output file").arg(m_fileName));
   }
}

void FileWriter::processItem(LedgerBudgetRoutineEntry const& entry)
{
   QTextStream out(m_file);
   out << "  routine " << entry.name();
   if (!entry.owner().isEmpty())
   {
      out << "  " << entry.owner();
   }
   out << endl;
   if (out.status() != QTextStream::Status::Ok)
   {
      die(QString("Unable to write to output file").arg(m_fileName));
   }
}

void FileWriter::processItem(LedgerComment const& comment)
{
   QTextStream out(m_file);
   out << ";" << comment.note() << endl;
   if (out.status() != QTextStream::Status::Ok)
   {
      die(QString("Unable to write to output file").arg(m_fileName));
   }
}

void FileWriter::processItem(LedgerReserve const& reserve)
{
   QTextStream out(m_file);
   out << reserve.date().toString(m_dateFormat) << " reserve";
   if (reserve.numEntries() > 1)
   {
      m_singleReserve = false;
      out << endl;
   }
   else
   {
      m_singleReserve = true;
   }
   if (out.status() != QTextStream::Status::Ok)
   {
      die(QString("Unable to write to output file").arg(m_fileName));
   }
}

void FileWriter::processItem(LedgerReserveEntry const& reserve)
{
   QTextStream out(m_file);
   if (m_singleReserve)
   {
      out << " ";
      m_singleReserve = false;
   }
   else
   {
      out << "  ";
   }
   if (reserve.isOwner())
   {
      out << "@";
   }
   out << reserve.category() << "  " << reserve.amount().toString() << endl;
   if (out.status() != QTextStream::Status::Ok)
   {
      die(QString("Unable to write to output file").arg(m_fileName));
   }
}

void FileWriter::processItem(LedgerTransaction const& transaction)
{
   QTextStream out(m_file);
   out << transaction.date().toString(m_dateFormat);
   out << " " << transaction.statusToString(transaction.status());
   out << " " << transaction.account() << "  ";
   uint numNotes = transaction.hasNote() ? 1 : 0;
   foreach (LedgerTransactionEntry const& entry, transaction.entries())
   {
      if (entry.hasNote())
      {
         numNotes++;
      }
   }
   if (transaction.entries().size() > 1 || numNotes > 1)
   {
      out << transaction.amount().toString();
      if (transaction.hasBalance())
      {
         out << " = " << transaction.balance().toString();
      }
      if (transaction.hasNote())
      {
         out << " ;" << transaction.note();
      }
      out << endl;
      foreach (LedgerTransactionEntry const& entry, transaction.entries())
      {
         out << "  ";
         if (entry.transfer())
         {
            out << "@";
         }
         out << entry.payee() << "  ";
         if (entry.hasCategory())
         {
            if (entry.isOwner())
            {
               out << "@";
            }
            out << entry.category() << "  ";
         }
         out << entry.amount().toString();
         if (entry.hasNote())
         {
            out << " ;" << entry.note();
         }
         out << endl;
      }
   }
   else
   {
      LedgerTransactionEntry entry(transaction.entries()[0]);
      if (entry.transfer())
      {
         out << "@";
      }
      out << entry.payee();
      if (entry.hasCategory())
      {
         out << "  ";
         if (entry.isOwner())
         {
            out << "@";
         }
         out << entry.category();
      }
      out << "  " << transaction.amount().toString();
      if (transaction.hasBalance())
      {
         out << " = " << transaction.balance().toString();
      }
      if (transaction.hasNote())
      {
         out << " ;" << transaction.note();
      }
      else if (entry.hasNote())
      {
         out << " ;" << entry.note();
      }
      out << endl;
   }
   if (out.status() != QTextStream::Status::Ok)
   {
      die(QString("Unable to write to output file").arg(m_fileName));
   }
}

void FileWriter::setDateFormat(QString const& dateFormat)
{
   m_dateFormat = dateFormat;
}

void FileWriter::start()
{
   if (!m_file || !m_file->open(QIODevice::WriteOnly | QIODevice::Text))
   {
      die(QString("Unable to open output file").arg(m_fileName));
   }
}
