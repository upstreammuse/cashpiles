#include "nativereader.h"

#include <QFile>
#include <QRegularExpression>
#include <QTextStream>

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

NativeReader::NativeReader(QObject *parent) :
   QObject(parent)
{
}

void NativeReader::readAll(QString const& filename)
{
   m_file = new QFile(filename, this);
   if (!m_file->open(QIODevice::ReadOnly | QIODevice::Text))
   {
      qWarning("Unable to open file %s", qPrintable(filename));
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
   NativeAccountCommand accountCommand(nextItemNum(), m_lineNum);
   accountCommand.setAccount(match.captured("account"));
   accountCommand.setCommand(match.captured("command"));
   accountCommand.setDate(match.captured("date"));
   emit account(accountCommand);
}

void NativeReader::processBudget(QRegularExpressionMatch& match)
{
   NativeBudgetCommand budgetCommand(nextItemNum(), m_lineNum);
   budgetCommand.setDate(match.captured("date"));
   forever
   {
      QString line(readLine());
      if ((match = budgetLineRx.match(line)).hasMatch())
      {
         budgetCommand.appendAllocation(match.captured("category"),
                                        match.captured("amount"));
      }
      else
      {
         unReadLine(line);
         break;
      }
   }
   emit budget(budgetCommand);
}

void NativeReader::processComment(QRegularExpressionMatch const& match)
{
   NativeComment comment_(nextItemNum(), m_lineNum);
   comment_.setNote(match.captured("note"));
   emit comment(comment_);
}

void NativeReader::processCompactTransaction(
      QRegularExpressionMatch const& match)
{
   NativeTransaction transaction_(nextItemNum(), m_lineNum);
   transaction_.setAccount(match.captured("account"));
   transaction_.setAmount(match.captured("amount"));
   if (!match.captured("balance").isEmpty())
   {
      transaction_.setBalance(match.captured("balance"));
   }
   transaction_.setCleared(match.captured("cleared"));
   transaction_.setDate(match.captured("date"));
   if (!match.captured("note").isEmpty())
   {
      transaction_.setNote(match.captured("note"));
   }
   NativeTransactionEntry entry;
   entry.setAmount(match.captured("amount"));
   entry.setCategory(match.captured("category"));
   entry.setPayee(match.captured("payee"));
   transaction_.appendEntry(entry);
   emit transaction(transaction_);
}

void NativeReader::processCompactTransactionOff(
      QRegularExpressionMatch const& match)
{
   NativeTransaction transaction_(nextItemNum(), m_lineNum);
   transaction_.setAccount(match.captured("account"));
   transaction_.setAmount(match.captured("amount"));
   if (!match.captured("balance").isEmpty())
   {
      transaction_.setBalance(match.captured("balance"));
   }
   transaction_.setCleared(match.captured("cleared"));
   transaction_.setDate(match.captured("date"));
   if (!match.captured("note").isEmpty())
   {
      transaction_.setNote(match.captured("note"));
   }
   NativeTransactionEntry entry;
   entry.setAmount(match.captured("amount"));
   entry.setPayee(match.captured("payee"));
   transaction_.appendEntry(entry);
   emit transaction(transaction_);
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
   NativeTransaction xact(nextItemNum(), m_lineNum);
   xact.setAccount(match.captured("account"));
   xact.setAmount(match.captured("amount"));
   if (!match.captured("balance").isEmpty())
   {
      xact.setBalance(match.captured("balance"));
   }
   xact.setCleared(match.captured("cleared"));
   xact.setDate(match.captured("date"));
   if (!match.captured("note").isEmpty())
   {
      xact.setNote(match.captured("note"));
   }

   forever
   {
      QString line(readLine());
      if ((match = txnLineRx.match(line)).hasMatch())
      {
         NativeTransactionEntry entry;
         entry.setAmount(match.captured("amount"));
         entry.setCategory(match.captured("category"));
         if (!match.captured("note").isEmpty())
         {
            entry.setNote(match.captured("note"));
         }
         entry.setPayee(match.captured("payee"));
         xact.appendEntry(entry);
      }
      else if ((match = txnLineOffRx.match(line)).hasMatch())
      {
         NativeTransactionEntry entry;
         entry.setAmount(match.captured("amount"));
         if (!match.captured("note").isEmpty())
         {
            entry.setNote(match.captured("note"));
         }
         entry.setPayee(match.captured("payee"));
         xact.appendEntry(entry);
      }
      else
      {
         unReadLine(line);
         break;
      }
   }

   emit transaction(xact);
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
