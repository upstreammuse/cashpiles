#include "nativereader.h"

#include <QFile>
#include <QRegularExpression>
#include <QTextStream>
#include "ledgeraccountcommand.h"
#include "ledgerallocation.h"
#include "ledgerbudget.h"
#include "ledgercomment.h"
#include "ledgertransaction.h"

namespace
{
   QString const CLEAR_RX("(?<cleared>\\*|\\!|\\?)");
   // TODO this needs to play well with the currency regexps somehow
   QString const CURR_RX(
         "(?<%1>(\\-\\$|\\$\\-|\\$)\\d{1,3}(\\,\\d{3})*\\.\\d{2,})");
   QString const DATE_RX("(?<date>\\d+[\\/\\.\\-]\\d+[\\/\\.\\-]\\d+)");
   QString const END_RX("\\s*$");
   QString const IDENT_RX("(?<%1>\\S(\\S| (?! ))*)");
   QString const INTERVAL_RX("(?<interval>\\+\\d+[dwmy])");
   QString const NOTE_RX(";(?<note>.*)");
   QString const OPTIONAL_RX("(%1)?");
   QString const SEP_RX("(\\s{2,}|\\t)\\s*");
   QString const SPACE_RX("\\s+");
   QString const START_RX("^");

   QRegularExpression const accountRx(
         START_RX + DATE_RX + SPACE_RX +
         "(?<command>on-budget|off-budget|close)" + SPACE_RX +
         IDENT_RX.arg("account") + END_RX);
   QRegularExpression const allocationRx(
         START_RX + DATE_RX + SPACE_RX + "allocation" + END_RX);
   QRegularExpression const allocationLineRx(
         START_RX + SEP_RX + IDENT_RX.arg("category") + SEP_RX +
         CURR_RX.arg("amount") + END_RX);
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
         SEP_RX + "\\d+%" + END_RX);
   QRegularExpression const budgetLineRoutineRx(
         START_RX + SEP_RX + "routine" + SPACE_RX + IDENT_RX.arg("category") +
         END_RX);
   QRegularExpression const commentRx(START_RX + NOTE_RX + END_RX);
   QRegularExpression const txnCompactRx(
         START_RX + DATE_RX + SPACE_RX + CLEAR_RX + SPACE_RX +
         IDENT_RX.arg("account") + SEP_RX + IDENT_RX.arg("payee") + SEP_RX +
         IDENT_RX.arg("category") + SEP_RX + CURR_RX.arg("amount") +
         OPTIONAL_RX.arg(SPACE_RX + QString("=") + SPACE_RX +
                         CURR_RX.arg("balance")) +
         OPTIONAL_RX.arg(SPACE_RX + NOTE_RX) + END_RX);
   QRegularExpression const txnCompactOffRx(
         START_RX + DATE_RX + SPACE_RX + CLEAR_RX + SPACE_RX +
         IDENT_RX.arg("account") + SEP_RX + IDENT_RX.arg("payee") + SEP_RX +
         CURR_RX.arg("amount") +
         OPTIONAL_RX.arg(SPACE_RX + QString("=") + SPACE_RX +
                         CURR_RX.arg("balance")) +
         OPTIONAL_RX.arg(SPACE_RX + NOTE_RX) + END_RX);
   QRegularExpression const txnRx(
         START_RX + DATE_RX + SPACE_RX + CLEAR_RX + SPACE_RX +
         IDENT_RX.arg("account") + SEP_RX + CURR_RX.arg("amount") +
         OPTIONAL_RX.arg(SPACE_RX + QString("=") + SPACE_RX +
                         CURR_RX.arg("balance")) +
         OPTIONAL_RX.arg(SPACE_RX + NOTE_RX) + END_RX);
   QRegularExpression const txnLineRx(
         START_RX + SEP_RX + IDENT_RX.arg("payee") + SEP_RX +
         IDENT_RX.arg("category") + SEP_RX + CURR_RX.arg("amount") +
         OPTIONAL_RX.arg(SPACE_RX + NOTE_RX) + END_RX);
   QRegularExpression const txnLineOffRx(
         START_RX + SEP_RX + IDENT_RX.arg("payee") + SEP_RX +
         CURR_RX.arg("amount") + OPTIONAL_RX.arg(SPACE_RX + NOTE_RX) + END_RX);
}

NativeReader::NativeReader(QString const& fileName, QObject *parent) :
   QObject(parent),
   m_file(new QFile(fileName, this)),
   m_fileName(fileName)
{
}

void NativeReader::readAll()
{
   if (!m_file || !m_file->open(QIODevice::ReadOnly | QIODevice::Text))
   {
      qWarning("Unable to open file %s", qPrintable(m_fileName));
   }
   while (m_file && !m_file->atEnd())
   {
      QString line(readLine());
      processLine(line);
   }
   m_file->close();
   emit finished();
}

void NativeReader::processAccount(QRegularExpressionMatch const& match)
{
   LedgerAccountCommand* accountCommand =
         new LedgerAccountCommand(m_fileName, m_lineNum);
   accountCommand->setAccount(match.captured("account"));
   accountCommand->setDate(parseDate(match.captured("date")));
   accountCommand->setMode(parseMode(match.captured("command")));
   emit item(accountCommand);
}

void NativeReader::processAllocation(QRegularExpressionMatch& match)
{
   LedgerAllocation* budgetCommand =
         new LedgerAllocation(m_fileName, m_lineNum);
   budgetCommand->setDate(parseDate(match.captured("date")));
   forever
   {
      QString line(readLine());
      if ((match = allocationLineRx.match(line)).hasMatch())
      {
         budgetCommand->appendAllocation(
                  match.captured("category"),
                  Currency(match.captured("amount"), m_lineNum));
      }
      else
      {
         unReadLine(line);
         break;
      }
   }
   emit item(budgetCommand);
}

void NativeReader::processBudget(QRegularExpressionMatch& match)
{
   LedgerBudget* budgetCommand = new LedgerBudget(m_fileName, m_lineNum);
   budgetCommand->setDate(parseDate(match.captured("date")));
   budgetCommand->setInterval(parseInterval(match.captured("interval")));
   forever
   {
      QString line(readLine());
      if ((match = budgetLineGoalRx.match(line)).hasMatch())
      {
         // TODO we have a goal category
      }
      else if ((match = budgetLineIncomeRx.match(line)).hasMatch())
      {
         // TODO we have income
      }
      else if ((match = budgetLineReserveAmountRx.match(line)).hasMatch())
      {
         // TODO we have reserve amount
      }
      else if ((match = budgetLineReservePercentRx.match(line)).hasMatch())
      {
         // TODO we have reserve percentage
      }
      else if ((match = budgetLineRoutineRx.match(line)).hasMatch())
      {
         // TODO we have routine
      }
      else
      {
         unReadLine(line);
         break;
      }
   }
   emit item(budgetCommand);
}

void NativeReader::processComment(QRegularExpressionMatch const& match)
{
   LedgerComment* comment = new LedgerComment(m_fileName, m_lineNum);
   comment->setNote(match.captured("note"));
   emit item(comment);
}

void NativeReader::processCompactTransaction(
      QRegularExpressionMatch const& match)
{
   LedgerTransaction* transaction =
         new LedgerTransaction(m_fileName, m_lineNum);
   transaction->setAccount(match.captured("account"));
   if (!match.captured("balance").isEmpty())
   {
      transaction->setBalance(Currency(match.captured("balance"), m_lineNum));
   }
   transaction->setCleared(match.captured("cleared") == "*");
   transaction->setDate(parseDate(match.captured("date")));
   if (!match.captured("note").isEmpty())
   {
      transaction->setNote(match.captured("note"));
   }

   LedgerTransactionEntry entry;
   entry.setAmount(Currency(match.captured("amount"), m_lineNum));
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

   emit item(transaction);
}

void NativeReader::processCompactTransactionOff(
      QRegularExpressionMatch const& match)
{
   LedgerTransaction* transaction =
         new LedgerTransaction(m_fileName, m_lineNum);
   transaction->setAccount(match.captured("account"));
   if (!match.captured("balance").isEmpty())
   {
      transaction->setBalance(Currency(match.captured("balance"), m_lineNum));
   }
   transaction->setCleared(match.captured("cleared") == "*");
   transaction->setDate(parseDate(match.captured("date")));
   if (!match.captured("note").isEmpty())
   {
      transaction->setNote(match.captured("note"));
   }

   LedgerTransactionEntry entry;
   entry.setAmount(Currency(match.captured("amount"), m_lineNum));
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

   emit item(transaction);
}

void NativeReader::processLine(QString const& line)
{
   QRegularExpressionMatch match;
   if ((match = accountRx.match(line)).hasMatch())
   {
      processAccount(match);
   }
   else if ((match = allocationRx.match(line)).hasMatch())
   {
      processAllocation(match);
   }
   else if ((match = budgetRx.match(line)).hasMatch())
   {
      processBudget(match);
   }
   else if ((match = commentRx.match(line)).hasMatch())
   {
      processComment(match);
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
      qWarning("invalid contents in file at line %d", m_lineNum);
      qWarning(qPrintable(line));
   }
}

void NativeReader::processTransaction(QRegularExpressionMatch& match)
{
   LedgerTransaction* xact = new LedgerTransaction(m_fileName, m_lineNum);
   xact->setAccount(match.captured("account"));
   if (!match.captured("balance").isEmpty())
   {
      xact->setBalance(Currency(match.captured("balance"), m_lineNum));
   }
   xact->setCleared(match.captured("cleared") == "*");
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
         entry.setAmount(Currency(match.captured("amount"), m_lineNum));
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
         entry.setAmount(Currency(match.captured("amount"), m_lineNum));
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

   emit item(xact);
}

QString NativeReader::readLine()
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

void NativeReader::unReadLine(QString const& line)
{
   --m_lineNum;
   m_lines.push_back(line);
}

QDate NativeReader::parseDate(QString const& date)
{
   // TODO let this be dynamic to accept dates in any unambiguous format
   QDate d(QDate::fromString(date, Qt::SystemLocaleShortDate));
   if (!d.isValid())
   {
      qWarning("Unable to read date '%s', expected something like '%s', line "
               "%d", qPrintable(date),
               qPrintable(QLocale::system().dateFormat(QLocale::ShortFormat)),
               m_lineNum);
      exit(EXIT_FAILURE);
   }
   return d;
}

Interval NativeReader::parseInterval(QString interval)
{
   QString periodStr = interval.right(1);
   interval.remove(0, 1).chop(1);
   bool ok = false;
   int number = interval.toInt(&ok);
   if (!ok)
   {
      qWarning("Failed to parse interval number '%s', line %d",
               qPrintable(interval), m_lineNum);
   }
   Interval::Period period;
   switch (periodStr[0].toLatin1())
   {
      case 'd':
         period = Interval::Period::DAYS;
         break;
      case 'w':
         period = Interval::Period::DAYS;
         number *= 7;
         break;
      case 'm':
         period = Interval::Period::MONTHS;
         break;
      case 'y':
         period = Interval::Period::YEARS;
         break;
      default:
         qWarning("Failed to get correct interval period from '%s', line %d",
                  qPrintable(periodStr), m_lineNum);
         break;
   }
   return Interval(number, period);
}

LedgerAccountCommand::Mode NativeReader::parseMode(QString const& command)
{
   if (command == "on-budget")
   {
      return LedgerAccountCommand::Mode::ON_BUDGET;
   }
   else if (command == "off-budget")
   {
      return LedgerAccountCommand::Mode::OFF_BUDGET;
   }
   else if (command == "close")
   {
      return LedgerAccountCommand::Mode::CLOSED;
   }
   else
   {
      qWarning("Unknown account command '%s', line %d",
               qPrintable(command), m_lineNum);
      return LedgerAccountCommand::Mode::CLOSED;
   }
}
