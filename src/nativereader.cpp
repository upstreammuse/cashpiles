#include "nativereader.h"

#include <QFile>
#include <QRegularExpression>
#include <QTextStream>
#include "ledgeraccountcommand.h"
#include "ledgerbudgetallocation.h"
#include "ledgercomment.h"
#include "ledgertransaction.h"

namespace
{
   QString const CLEAR_RX("(?<cleared>\\*|\\!)");
   QString const CURR_RX(
         "(?<%1>(\\-\\$|\\$\\-|\\$)\\d{1,3}(\\,\\d{3})*\\.\\d{2,})");
   QString const DATE_RX("(?<date>\\d+[\\/\\.\\-]\\d+[\\/\\.\\-]\\d+)");
   QString const END_RX("\\s*$");
   QString const IDENT_RX("(?<%1>\\S(\\S| (?! ))*)");
   QString const NOTE_RX(";(?<note>.*)");
   QString const OPTIONAL_RX("(%1)?");
   QString const SEP_RX("(\\s{2,}|\\t)\\s*");
   QString const SPACE_RX("\\s+");
   QString const START_RX("^");

   QRegularExpression const accountRx(
         START_RX + DATE_RX + SPACE_RX +
         "(?<command>on-budget|off-budget|close)" + SPACE_RX +
         IDENT_RX.arg("account") + END_RX);
   QRegularExpression const budgetRx(
         START_RX + DATE_RX + SPACE_RX + "budget" + END_RX);
   QRegularExpression const budgetLineRx(
         START_RX + SEP_RX + IDENT_RX.arg("category") + SEP_RX +
         CURR_RX.arg("amount") + END_RX);
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

NativeReader::NativeReader(QString const& filename, QObject *parent) :
   QObject(parent),
   m_filename(filename)
{
}

void NativeReader::readAll()
{
   m_file = new QFile(m_filename, this);
   if (!m_file->open(QIODevice::ReadOnly | QIODevice::Text))
   {
      qWarning("Unable to open file %s", qPrintable(m_filename));
   }
   while (m_file && !m_file->atEnd())
   {
      QString line(readLine());
      processLine(line);
   }
   m_file->close();
   emit finished();
}

int NativeReader::nextItemNum()
{
   return ++m_itemNum;
}

void NativeReader::processAccount(QRegularExpressionMatch const& match)
{
   // TODO fix this crap
   LedgerAccountCommand* accountCommand =
         new LedgerAccountCommand(((QFile*)m_file)->fileName(), m_lineNum);
   accountCommand->setAccount(match.captured("account"));
   accountCommand->setDate(parseDate(match.captured("date"), m_lineNum));
   accountCommand->setMode(parseMode(match.captured("command"), m_lineNum));
   emit item(accountCommand);
}

void NativeReader::processBudget(QRegularExpressionMatch& match)
{
   LedgerBudgetAllocation* budgetCommand = new LedgerBudgetAllocation("TODO", m_lineNum);
   budgetCommand->setDate(parseDate(match.captured("date"), m_lineNum));
   forever
   {
      QString line(readLine());
      if ((match = budgetLineRx.match(line)).hasMatch())
      {
         budgetCommand->appendAllocation(match.captured("category"),
                                         parseCurrency(match.captured("amount"), m_lineNum));
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
   LedgerComment* comment = new LedgerComment("TODO", m_lineNum);
   comment->setNote(match.captured("note"));
   emit item(comment);
}

void NativeReader::processCompactTransaction(
      QRegularExpressionMatch const& match)
{
   // TODO fix this
   LedgerTransaction* transaction = new LedgerTransaction(((QFile*)m_file)->fileName(), m_lineNum);
   transaction->setAccount(match.captured("account"));
   if (!match.captured("balance").isEmpty())
   {
      transaction->setBalance(parseCurrency(match.captured("balance"), m_lineNum));
   }
   transaction->setCleared(match.captured("cleared") == "*");
   transaction->setDate(parseDate(match.captured("date"), m_lineNum));
   if (!match.captured("note").isEmpty())
   {
      transaction->setNote(match.captured("note"));
   }

   LedgerTransactionEntry entry;
   entry.setAmount(parseCurrency(match.captured("amount"), m_lineNum));
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
   // TODO fix this
   LedgerTransaction* transaction = new LedgerTransaction(((QFile*)m_file)->fileName(), m_lineNum);
   transaction->setAccount(match.captured("account"));
   if (!match.captured("balance").isEmpty())
   {
      transaction->setBalance(parseCurrency(match.captured("balance"), m_lineNum));
   }
   transaction->setCleared(match.captured("cleared") == "*");
   transaction->setDate(parseDate(match.captured("date"), m_lineNum));
   if (!match.captured("note").isEmpty())
   {
      transaction->setNote(match.captured("note"));
   }

   LedgerTransactionEntry entry;
   entry.setAmount(parseCurrency(match.captured("amount"), m_lineNum));
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
   // TODO fix this
   LedgerTransaction* xact = new LedgerTransaction(((QFile*)m_file)->fileName(), m_lineNum);

   xact->setAccount(match.captured("account"));
   if (!match.captured("balance").isEmpty())
   {
      xact->setBalance(parseCurrency(match.captured("balance"), m_lineNum));
   }
   xact->setCleared(match.captured("cleared") == "*");
   xact->setDate(parseDate(match.captured("date"), m_lineNum));
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
         entry.setAmount(parseCurrency(match.captured("amount"), m_lineNum));
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
         entry.setAmount(parseCurrency(match.captured("amount"), m_lineNum));
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

int NativeReader::parseCurrency(QString curr, int line)
{
   curr.remove(QLocale::system().currencySymbol());
   curr.remove(QLocale::system().groupSeparator());
   int decimalIndex = curr.indexOf(QLocale::system().decimalPoint());
   int decimalDigits = curr.length() - decimalIndex - 1;
   if (m_decimalDigits == -1)
   {
      m_decimalDigits = decimalDigits;
   }
   else if (m_decimalDigits != decimalDigits)
   {
      qWarning("Inconsistent number of decimal digits, line %d", line);
      exit(EXIT_FAILURE);
   }
   curr.remove(QLocale::system().decimalPoint());
   bool success;
   int retval = curr.toInt(&success);
   if (!success)
   {
      qWarning("Could not understand currency, line %d", line);
      exit(EXIT_FAILURE);
   }
   return retval;
}

QDate NativeReader::parseDate(QString const& date, int line)
{
   // TODO let this be dynamic to accept dates in any unambiguous format
   QDate d(QDate::fromString(date, Qt::SystemLocaleShortDate));
   if (!d.isValid())
   {
      qWarning("Unable to read date '%s', expected something like '%s', line "
               "%d", qPrintable(date),
               qPrintable(QLocale::system().dateFormat(QLocale::ShortFormat)),
               line);
      exit(EXIT_FAILURE);
   }
   return d;
}

LedgerAccountCommand::Mode NativeReader::parseMode(QString const& command,
                                                   int line)
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
               qPrintable(command), line);
      return LedgerAccountCommand::Mode::CLOSED;
   }
}
