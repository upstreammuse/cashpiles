#include "filereader.h"

#include <QDebug>
#include "ledger.h"
#include "ledgeraccount.h"
#include "ledgerbudget.h"
#include "ledgerbudgetentry.h"
#include "ledgercomment.h"
#include "ledgerreserve.h"
#include "ledgertransaction.h"

namespace
{
   QString const CLEAR_RX("(?<cleared>\\*|\\!|\\?)");
   QString const CURR_RX(QString("(?<%5>(\\%1|\\%2|\\%3|\\%4|\\d)+)")
                         .arg(QLocale::system().currencySymbol())
                         .arg(QLocale::system().negativeSign())
                         .arg(QLocale::system().groupSeparator())
                         .arg(QLocale::system().decimalPoint()));
   QString const DATE_RX("(?<date>\\d+[\\/\\.\\-]\\d+[\\/\\.\\-]\\d+)");
   QString const END_RX("\\s*$");
   QString const IDENT_RX("(?<%1>\\S(\\S| (?! ))*)");
   QString const INTERVAL_RX("(?<interval>\\+\\d+[dwmy])");
   QString const NOTE_RX(";(?<note>.*)");
   QString const OPTIONAL_RX("(%1)?");
   QString const PERCENT_RX("(?<%1>\\d+)%");
   QString const SEP_RX("(\\s{2,}|\\t)\\s*");
   QString const SPACE_RX("\\s+");
   QString const START_RX("^");

   QRegularExpression const accountRx(
         START_RX + DATE_RX + SPACE_RX +
         "(?<command>on-budget|off-budget|close)" + SPACE_RX +
         IDENT_RX.arg("account") + END_RX);
   QRegularExpression const budgetRx(
         START_RX + DATE_RX + SPACE_RX + "budget" + SPACE_RX + INTERVAL_RX +
         END_RX);
   QRegularExpression const budgetLineGoalRx(
         START_RX + SEP_RX + "goal" + SPACE_RX + IDENT_RX.arg("category") +
         END_RX);
   QRegularExpression const budgetLineIncomeRx(
         START_RX + SEP_RX + "income" + SPACE_RX + IDENT_RX.arg("category") +
         END_RX);
   QRegularExpression const budgetLineReserveAmountRx(
         START_RX + SEP_RX + "reserve" + SPACE_RX + IDENT_RX.arg("category") +
         SEP_RX + CURR_RX.arg("amount") + SPACE_RX + INTERVAL_RX + END_RX);
   QRegularExpression const budgetLineReservePercentRx(
         START_RX + SEP_RX + "reserve" + SPACE_RX + IDENT_RX.arg("category") +
         SEP_RX + PERCENT_RX.arg("percentage") + END_RX);
   QRegularExpression const budgetLineRoutineRx(
         START_RX + SEP_RX + "routine" + SPACE_RX + IDENT_RX.arg("category") +
         END_RX);
   QRegularExpression const commentRx(START_RX + NOTE_RX + END_RX);
   QRegularExpression const reserveRx(
         START_RX + DATE_RX + SPACE_RX + "reserve" + SPACE_RX +
         IDENT_RX.arg("category") + SEP_RX + CURR_RX.arg("amount") + END_RX);
   QRegularExpression const txnCompactRx(
         START_RX + DATE_RX + SPACE_RX + CLEAR_RX + SPACE_RX +
         IDENT_RX.arg("account") + SEP_RX + IDENT_RX.arg("payee") + SEP_RX +
         IDENT_RX.arg("category") + SEP_RX + CURR_RX.arg("amount") +
         OPTIONAL_RX.arg(SPACE_RX + "=" + SPACE_RX + CURR_RX.arg("balance")) +
         OPTIONAL_RX.arg(SPACE_RX + NOTE_RX) + END_RX);
   QRegularExpression const txnCompactOffRx(
         START_RX + DATE_RX + SPACE_RX + CLEAR_RX + SPACE_RX +
         IDENT_RX.arg("account") + SEP_RX + IDENT_RX.arg("payee") + SEP_RX +
         CURR_RX.arg("amount") +
         OPTIONAL_RX.arg(SPACE_RX + "=" + SPACE_RX + CURR_RX.arg("balance")) +
         OPTIONAL_RX.arg(SPACE_RX + NOTE_RX) + END_RX);
   QRegularExpression const txnRx(
         START_RX + DATE_RX + SPACE_RX + CLEAR_RX + SPACE_RX +
         IDENT_RX.arg("account") + SEP_RX + CURR_RX.arg("amount") +
         OPTIONAL_RX.arg(SPACE_RX + "=" + SPACE_RX + CURR_RX.arg("balance")) +
         OPTIONAL_RX.arg(SPACE_RX + NOTE_RX) + END_RX);
   QRegularExpression const txnLineRx(
         START_RX + SEP_RX + IDENT_RX.arg("payee") + SEP_RX +
         IDENT_RX.arg("category") + SEP_RX + CURR_RX.arg("amount") +
         OPTIONAL_RX.arg(SPACE_RX + NOTE_RX) + END_RX);
   QRegularExpression const txnLineOffRx(
         START_RX + SEP_RX + IDENT_RX.arg("payee") + SEP_RX +
         CURR_RX.arg("amount") + OPTIONAL_RX.arg(SPACE_RX + NOTE_RX) + END_RX);
}

FileReader::FileReader(QString const& fileName, Ledger& ledger, QObject* parent) :
   QObject(parent),
   m_file(new QFile(fileName, this)),
   m_fileName(fileName),
   m_ledger(ledger)
{
}

bool FileReader::readAll()
{
   m_success = true;
   if (!m_file || !m_file->open(QIODevice::ReadOnly | QIODevice::Text))
   {
      failed("Unable to open file");
   }
   while (hasLines())
   {
      QString line(readLine());
      processLine(line);
   }
   m_file->close();
   return m_success;
}

void FileReader::setDateFormat(QString const& dateFormat)
{
   m_dateFormat = dateFormat;
}

void FileReader::processAccount(QRegularExpressionMatch const& match)
{
   QSharedPointer<LedgerAccount> account(
            new LedgerAccount(m_fileName, m_lineNum));
   account->setName(match.captured("account"));
   account->setDate(parseDate(match.captured("date")));
   bool ok;
   account->setMode(
            LedgerAccount::modeFromString(match.captured("command"), &ok));
   if (!ok)
   {
      failed(QString("Unknown account command '%1'")
             .arg(match.captured("command")));
   }
   m_ledger.appendItem(account);
}

void FileReader::processBudget(QRegularExpressionMatch& match)
{
   QSharedPointer<LedgerBudget> budget(new LedgerBudget(m_fileName, m_lineNum));
   budget->setDate(parseDate(match.captured("date")));
   bool ok;
   budget->setInterval(Interval::fromString(match.captured("interval"), &ok));
   if (!ok)
   {
      failed(QString("Unable to read interval '%1'")
             .arg(match.captured("interval")));
   }

   forever
   {
      QString line(readLine());
      if ((match = budgetLineGoalRx.match(line)).hasMatch())
      {
         QSharedPointer<LedgerBudgetGoalEntry> entry(new LedgerBudgetGoalEntry);
         entry->setName(match.captured("category"));
         budget->appendEntry(entry);
      }
      else if ((match = budgetLineIncomeRx.match(line)).hasMatch())
      {
         QSharedPointer<LedgerBudgetIncomeEntry> entry(
                  new LedgerBudgetIncomeEntry);
         entry->setName(match.captured("category"));
         budget->appendEntry(entry);
      }
      else if ((match = budgetLineReserveAmountRx.match(line)).hasMatch())
      {
         QSharedPointer<LedgerBudgetReserveAmountEntry> entry(
                  new LedgerBudgetReserveAmountEntry);
         entry->setName(match.captured("category"));
         bool ok;
         entry->setAmount(Currency::fromString(match.captured("amount"), &ok));
         if (!ok)
         {
            qDebug() << "Unable to parse currency" << m_fileName << m_lineNum
                     << match.captured("amount");
            failedCurrency(match.captured("amount"));
         }
         entry->setInterval(
                  Interval::fromString(match.captured("interval"), &ok));
         if (!ok)
         {
            failed(QString("Unable to read interval '%1'")
                   .arg(match.captured("interval")));
         }
         budget->appendEntry(entry);
      }
      else if ((match = budgetLineReservePercentRx.match(line)).hasMatch())
      {
         QSharedPointer<LedgerBudgetReservePercentEntry> entry(
                  new LedgerBudgetReservePercentEntry);
         entry->setName(match.captured("category"));
         entry->setPercentage(match.captured("percentage").toUInt());
         budget->appendEntry(entry);
      }
      else if ((match = budgetLineRoutineRx.match(line)).hasMatch())
      {
         QSharedPointer<LedgerBudgetRoutineEntry> entry(
                  new LedgerBudgetRoutineEntry);
         entry->setName(match.captured("category"));
         budget->appendEntry(entry);
      }
      else
      {
         unReadLine(line);
         break;
      }
   }

   m_ledger.appendItem(budget);
}

void FileReader::processComment(QRegularExpressionMatch const& match)
{
   QSharedPointer<LedgerComment> comment(
            new LedgerComment(m_fileName, m_lineNum));
   comment->setNote(match.captured("note"));
   m_ledger.appendItem(comment);
}

void FileReader::processCompactTransaction(QRegularExpressionMatch const& match)
{
   QSharedPointer<LedgerTransaction> transaction(
            new LedgerTransaction(m_fileName, m_lineNum));
   transaction->setAccount(match.captured("account"));
   if (!match.captured("balance").isEmpty())
   {
      bool ok;
      transaction->setBalance(
               Currency::fromString(match.captured("balance"), &ok));
      if (!ok)
      {
         failedCurrency(match.captured("balance"));
      }
   }

   bool ok;
   transaction->setStatus(
            transaction->statusFromString(match.captured("cleared"), &ok));
   if (!ok)
   {
      failed(QString("Could not read transaction status '%1'")
             .arg(match.captured("cleared")));
   }
   transaction->setDate(parseDate(match.captured("date")));
   if (!match.captured("note").isEmpty())
   {
      transaction->setNote(match.captured("note"));
   }

   LedgerTransactionEntry entry;
   entry.setAmount(Currency::fromString(match.captured("amount"), &ok));
   if (!ok)
   {
      failedCurrency(match.captured("amount"));
   }
   entry.setCategory(match.captured("category"));
   if (match.captured("payee").startsWith("@"))
   {
      entry.setPayee(match.captured("payee").mid(1));
      entry.setTransfer(true);
   }
   else
   {
      entry.setPayee(match.captured("payee"));
   }
   transaction->appendEntry(entry);

   m_ledger.appendItem(transaction);
}

void FileReader::processCompactTransactionOff(
      QRegularExpressionMatch const& match)
{
   QSharedPointer<LedgerTransaction> transaction(
            new LedgerTransaction(m_fileName, m_lineNum));
   transaction->setAccount(match.captured("account"));
   if (!match.captured("balance").isEmpty())
   {
      bool ok;
      transaction->setBalance(
               Currency::fromString(match.captured("balance"), &ok));
      if (!ok)
      {
         failedCurrency(match.captured("balance"));
      }
   }
   bool ok;
   transaction->setStatus(
            transaction->statusFromString(match.captured("cleared"), &ok));
   if (!ok)
   {
      failed(QString("Could not read transaction status '%1'")
             .arg(match.captured("cleared")));
   }
   transaction->setDate(parseDate(match.captured("date")));
   if (!match.captured("note").isEmpty())
   {
      transaction->setNote(match.captured("note"));
   }

   LedgerTransactionEntry entry;
   entry.setAmount(Currency::fromString(match.captured("amount"), &ok));
   if (!ok)
   {
      failedCurrency(match.captured("amount"));
   }
   if (match.captured("payee").startsWith("@"))
   {
      entry.setPayee(match.captured("payee").mid(1));
      entry.setTransfer(true);
   }
   else
   {
      entry.setPayee(match.captured("payee"));
   }
   transaction->appendEntry(entry);

   m_ledger.appendItem(transaction);
}

void FileReader::processLine(QString const& line)
{
   QRegularExpressionMatch match;
   if ((match = accountRx.match(line)).hasMatch())
   {
      processAccount(match);
   }
   else if ((match = budgetRx.match(line)).hasMatch())
   {
      processBudget(match);
   }
   else if ((match = commentRx.match(line)).hasMatch())
   {
      processComment(match);
   }
   else if ((match = reserveRx.match(line)).hasMatch())
   {
      processReserve(match);
   }
   else if ((match = txnCompactRx.match(line)).hasMatch())
   {
      processCompactTransaction(match);
   }
   else if ((match = txnCompactOffRx.match(line)).hasMatch())
   {
      processCompactTransactionOff(match);
   }
   else if ((match = txnRx.match(line)).hasMatch())
   {
      processTransaction(match);
   }
   else if (!line.trimmed().isEmpty())
   {
      failed(QString("Invalid contents '%1'").arg(line));
   }
}

void FileReader::processReserve(const QRegularExpressionMatch &match)
{
   QSharedPointer<LedgerReserve> reserve(
            new LedgerReserve(m_fileName, m_lineNum));
   reserve->setDate(parseDate(match.captured("date")));
   reserve->setCategory(match.captured("category"));

   bool ok;
   reserve->setAmount(Currency::fromString(match.captured("amount"), &ok));
   if (!ok)
   {
      failedCurrency(match.captured("amount"));
   }

   m_ledger.appendItem(reserve);
}

void FileReader::processTransaction(QRegularExpressionMatch& match)
{
   QSharedPointer<LedgerTransaction> xact(
            new LedgerTransaction(m_fileName, m_lineNum));
   xact->setAccount(match.captured("account"));
   if (!match.captured("balance").isEmpty())
   {
      bool ok;
      xact->setBalance(Currency::fromString(match.captured("balance"), &ok));
      if (!ok)
      {
         failedCurrency(match.captured("balance"));
      }
   }
   bool ok;
   xact->setStatus(xact->statusFromString(match.captured("cleared"), &ok));
   if (!ok)
   {
      failed(QString("Could not read transaction status '%1'")
             .arg(match.captured("cleared")));
   }
   xact->setDate(parseDate(match.captured("date")));
   if (!match.captured("note").isEmpty())
   {
      xact->setNote(match.captured("note"));
   }

   forever
   {
      QString line(readLine());
      if ((match = txnLineRx.match(line)).hasMatch())
      {
         LedgerTransactionEntry entry;
         bool ok;
         entry.setAmount(Currency::fromString(match.captured("amount"), &ok));
         if (!ok)
         {
            failedCurrency(match.captured("amount"));
         }
         entry.setCategory(match.captured("category"));
         if (!match.captured("note").isEmpty())
         {
            entry.setNote(match.captured("note"));
         }
         if (match.captured("payee").startsWith("@"))
         {
            entry.setPayee(match.captured("payee").mid(1));
            entry.setTransfer(true);
         }
         else
         {
            entry.setPayee(match.captured("payee"));
         }
         xact->appendEntry(entry);
      }
      else if ((match = txnLineOffRx.match(line)).hasMatch())
      {
         LedgerTransactionEntry entry;
         bool ok;
         entry.setAmount(Currency::fromString(match.captured("amount"), &ok));
         if (!ok)
         {
            failedCurrency(match.captured("amount"));
         }
         if (!match.captured("note").isEmpty())
         {
            entry.setNote(match.captured("note"));
         }
         if (match.captured("payee").startsWith("@"))
         {
            entry.setPayee(match.captured("payee").mid(1));
            entry.setTransfer(true);
         }
         else
         {
            entry.setPayee(match.captured("payee"));
         }
         xact->appendEntry(entry);
      }
      else
      {
         unReadLine(line);
         break;
      }
   }

   m_ledger.appendItem(xact);
}

bool FileReader::hasLines()
{
   return !m_lines.empty() || (m_file && !m_file->atEnd());
}

QString FileReader::readLine()
{
   if (!m_lines.empty())
   {
      QString line(m_lines.back());
      m_lines.pop_back();
      ++m_lineNum;
      return line;
   }
   else if (m_file)
   {
      ++m_lineNum;
      return m_file->readLine();
   }
   else
   {
      return QString();
   }
}

void FileReader::unReadLine(QString const& line)
{
   --m_lineNum;
   m_lines.push_back(line);
}

void FileReader::failed(QString const& message)
{
   qDebug() << QString("File '%1', Line %2: %3")
               .arg(m_fileName).arg(m_lineNum).arg(message);
   m_success = false;
}

void FileReader::failedCurrency(QString const& currency)
{
   failed(QString("Unable to parse currency '%1'").arg(currency));
}

QDate FileReader::parseDate(QString const& date)
{
   QDate d(QDate::fromString(date, m_dateFormat));
   if (!d.isValid())
   {
      failed(QString("Unable to read date '%1', expected '%2'")
             .arg(date).arg(m_dateFormat));
   }
   return d;
}
