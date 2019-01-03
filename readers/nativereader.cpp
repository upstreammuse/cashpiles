#include "nativereader.h"

#include <QFile>
#include <QRegularExpression>
#include <QTextStream>
#include "kernel/ledgeraccountcommand.h"
#include "kernel/ledgerbudget.h"
#include "kernel/ledgercomment.h"
#include "kernel/ledgertransaction.h"

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
   QRegularExpression const blockCommentRx(START_RX + ";;;" + END_RX);
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
      emit message(QString("Unable to open file '%1'").arg(m_fileName));
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

void NativeReader::processBudget(QRegularExpressionMatch& match)
{
   LedgerBudget* budgetCommand = new LedgerBudget(m_fileName, m_lineNum);
   budgetCommand->setDate(parseDate(match.captured("date")));
   budgetCommand->setInterval(parseInterval(match.captured("interval")));
   forever
   {
      LedgerBudget::Category category;
      QString line(readLine());
      if ((match = budgetLineGoalRx.match(line)).hasMatch())
      {
         category.type = LedgerBudget::Category::Type::GOAL;
      }
      else if ((match = budgetLineIncomeRx.match(line)).hasMatch())
      {
         category.type = LedgerBudget::Category::Type::INCOME;
      }
      else if ((match = budgetLineReserveAmountRx.match(line)).hasMatch())
      {
         category.type = LedgerBudget::Category::Type::RESERVE_AMOUNT;
         bool ok = false;
         category.amount = Currency::fromString(match.captured("amount"), &ok);
         if (!ok)
         {
            LedgerComment item(m_fileName, m_lineNum);
            emit message(item,
                         QString("Unable to parse '%1' as currency")
                         .arg(match.captured("amount")));
         }
         category.interval = parseInterval(match.captured("interval"));
      }
      else if ((match = budgetLineReservePercentRx.match(line)).hasMatch())
      {
         category.type = LedgerBudget::Category::Type::RESERVE_PERCENT;
         category.percentage = match.captured("percentage").toInt();
         if (category.percentage < 1 || category.percentage > 100)
         {
            LedgerComment item(m_fileName, m_lineNum);
            emit message(item, "Percentage allocation is out of range");
         }
      }
      else if ((match = budgetLineRoutineRx.match(line)).hasMatch())
      {
         category.type = LedgerBudget::Category::Type::ROUTINE;
      }
      else
      {
         unReadLine(line);
         break;
      }
      budgetCommand->insertCategory(match.captured("category"), category);
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
      bool ok = false;
      transaction->setBalance(Currency::fromString(match.captured("balance"), &ok));
      if (!ok)
      {
         LedgerComment item(m_fileName, m_lineNum);
         emit message(item,
                      QString("Unable to parse '%1' as currency")
                      .arg(match.captured("balance")));
      }
   }
   transaction->setCleared(match.captured("cleared") == "*");
   transaction->setDate(parseDate(match.captured("date")));
   if (!match.captured("note").isEmpty())
   {
      transaction->setNote(match.captured("note"));
   }

   LedgerTransactionEntry entry;
   bool ok;
   entry.setAmount(Currency::fromString(match.captured("amount"), &ok));
   if (!ok)
   {
      LedgerComment item(m_fileName, m_lineNum);
      emit message(item,
                   QString("Unable to parse '%1' as currency")
                   .arg(match.captured("amount")));
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
      bool ok;
      transaction->setBalance(Currency::fromString(match.captured("balance"), &ok));
      if (!ok)
      {
         LedgerComment item(m_fileName, m_lineNum);
         emit message(item,
                      QString("Unable to parse '%1' as currency")
                      .arg(match.captured("balance")));
      }
   }
   transaction->setCleared(match.captured("cleared") == "*");
   transaction->setDate(parseDate(match.captured("date")));
   if (!match.captured("note").isEmpty())
   {
      transaction->setNote(match.captured("note"));
   }

   LedgerTransactionEntry entry;
   bool ok;
   entry.setAmount(Currency::fromString(match.captured("amount"), &ok));
   if (!ok)
   {
      LedgerComment item(m_fileName, m_lineNum);
      emit message(item,
                   QString("Unable to parse '%1' as currency")
                   .arg(match.captured("amount")));
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

   emit item(transaction);
}

void NativeReader::processLine(QString const& line)
{
   QRegularExpressionMatch match;
   if ((match = blockCommentRx.match(line)).hasMatch())
   {
      forever
      {
         QString line = readLine();
         if ((match = blockCommentRx.match(line)).hasMatch())
         {
            break;
         }
      }
   }
   else if ((match = accountRx.match(line)).hasMatch())
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
      LedgerComment item(m_fileName, m_lineNum);
      emit message(item, "Invalid contents in file");
   }
}

void NativeReader::processTransaction(QRegularExpressionMatch& match)
{
   LedgerTransaction* xact = new LedgerTransaction(m_fileName, m_lineNum);
   xact->setAccount(match.captured("account"));
   if (!match.captured("balance").isEmpty())
   {
      bool ok;
      xact->setBalance(Currency::fromString(match.captured("balance"), &ok));
      if (!ok)
      {
         LedgerComment item(m_fileName, m_lineNum);
         emit message(item,
                      QString("Unable to parse '%1' as currency")
                      .arg(match.captured("balance")));
      }
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
         bool ok;
         entry.setAmount(Currency::fromString(match.captured("amount"), &ok));
         if (!ok)
         {
            LedgerComment item(m_fileName, m_lineNum);
            emit message(item,
                         QString("Unable to parse '%1' as currency")
                         .arg(match.captured("amount")));
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
            LedgerComment item(m_fileName, m_lineNum);
            emit message(item,
                         QString("Unable to parse '%1' as currency")
                         .arg(match.captured("amount")));
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
      LedgerComment item(m_fileName, m_lineNum);
      emit message(item,
                   QString("Unable to read date '%1', expected something like "
                           "'%2'")
                   .arg(date)
                   .arg(QLocale::system().dateFormat(QLocale::ShortFormat)));
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
      LedgerComment item(m_fileName, m_lineNum);
      emit message(item,
                   QString("Failed to parse interval number '%1'")
                   .arg(interval));
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
         emit message(LedgerComment(m_fileName, m_lineNum),
                      QString("Failed to get correct interval period from '%1'")
                      .arg(periodStr));
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
      emit message(LedgerComment(m_fileName, m_lineNum),
                   QString("Unknown account command '%1'")
                   .arg(command));
      return LedgerAccountCommand::Mode::CLOSED;
   }
}