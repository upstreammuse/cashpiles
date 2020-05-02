#include "filereader.h"

#include <regex>
#include <sstream>
#include "cashpiles.h"
#include "date.h"
#include "ledger.h"
#include "ledgeraccount.h"
#include "ledgerblank.h"
#include "ledgerbudget.h"
#include "ledgerbudgetcancelentry.h"
#include "ledgerbudgetcloseentry.h"
#include "ledgerbudgetentry.h"
#include "ledgerbudgetgoalentry.h"
#include "ledgerbudgetgoalsentry.h"
#include "ledgerbudgetincomeentry.h"
#include "ledgerbudgetreserveamountentry.h"
#include "ledgerbudgetreservepercententry.h"
#include "ledgerbudgetroutineentry.h"
#include "ledgerbudgetwithholdingentry.h"
#include "ledgercomment.h"
#include "ledgertransaction.h"

using std::make_shared;

struct FileReaderRegEx
{
   std::string const CLEAR_RX;
   std::string const CURR_RX;
   std::string const DATE_RX;
   std::string const END_RX;
   std::string const IDENT_RX;
   std::string const INTERVAL_RX;
   std::string const NOTE_RX;
   std::string const PERCENT_RX;
   std::string const SEP_RX;
   std::string const SPACE_RX;
   std::string const START_RX;

   std::regex const accountRx;
   std::regex const budgetRx;
   std::regex const budgetLineCancelRx;
   std::regex const budgetLineCloseRx;
   std::regex const budgetLineGoalRx;
   std::regex const budgetLineGoalsRx;
   std::regex const budgetLineIncomeRx;
   std::regex const budgetLineReserveAmountRx;
   std::regex const budgetLineReservePercentRx;
   std::regex const budgetLineRoutineRx;
   std::regex const budgetLineWithholdingRx;
   std::regex const commentRx;
   std::regex const txnCompactRx;
   std::regex const txnCompactOffRx;
   std::regex const txnRx;
   std::regex const txnLineRx;
   std::regex const txnLineOffRx;

   std::string currencyRx()
   {
      std::stringstream retval;
      struct lconv* lc = localeconv();
      retval << "((?:";

      std::string symbol(lc->currency_symbol);
      // make sure ascii currency symbols don't interfere with regex
      for (char c : symbol)
      {
         if (c & 0x80)
         {
            retval << c;
         }
         else
         {
            retval << '\\' << c;
         }
      }
      retval << '|';

      std::string sep(lc->mon_thousands_sep);
      // make sure ascii separator symbols don't interfere with regex
      for (char c : sep)
      {
         if (c & 0x80)
         {
            retval << c;
         }
         else
         {
            retval << '\\' << c;
         }
      }
      retval << '|';

      std::string decimal(lc->mon_decimal_point);
      for (char c : decimal)
      {
         if (c & 0x80)
         {
            retval << c;
         }
         else
         {
            retval << '\\' << c;
         }
      }
      retval << '|';

      std::string negative(lc->negative_sign);
      for (char c : negative)
      {
         if (c & 0x80)
         {
            retval << c;
         }
         else
         {
            retval << '\\' << c;
         }
      }
      retval << '|';

      retval << "\\d)+)";
      return retval.str();
   }

   std::string optional(std::string const& item)
   {
      std::stringstream ss;
      ss << "(?:" << item << ")?";
      return ss.str();
   }

   FileReaderRegEx() :
      CLEAR_RX("(\\*|\\!|\\?)"),
      CURR_RX(currencyRx()),
      DATE_RX("(\\d+[\\/\\.\\-]\\d+[\\/\\.\\-]\\d+)"),
      END_RX("\\s*$"),
      IDENT_RX("(\\S(?:\\S| (?! ))*)"),
      INTERVAL_RX("(\\+\\d+[dwmy])"),
      NOTE_RX(";(.*)"),
      PERCENT_RX("(\\d+)%"),
      SEP_RX("(?:\\s{2,}|\\t)\\s*"),
      SPACE_RX("\\s+"),
      START_RX("^"),
      accountRx(
         START_RX + DATE_RX + SPACE_RX + "(on-budget|off-budget|close)" +
         SPACE_RX + IDENT_RX + END_RX),
      budgetRx(
         START_RX + DATE_RX + SPACE_RX + "budget" + SPACE_RX + INTERVAL_RX +
         END_RX),
      budgetLineCancelRx(
         START_RX + SEP_RX + "cancel" + SPACE_RX + IDENT_RX + SEP_RX +
         IDENT_RX + END_RX),
      budgetLineCloseRx(
         START_RX + SEP_RX + "close" + SPACE_RX + IDENT_RX + END_RX),
      budgetLineGoalRx(
         START_RX + SEP_RX + "goal" + SPACE_RX + IDENT_RX + SEP_RX + IDENT_RX +
         SEP_RX + CURR_RX + SPACE_RX + DATE_RX + optional(SPACE_RX + NOTE_RX) +
         END_RX),
      budgetLineGoalsRx(
         START_RX + SEP_RX + "goals" + SPACE_RX + IDENT_RX +
         optional(SEP_RX + IDENT_RX) + END_RX),
      budgetLineIncomeRx(
         START_RX + SEP_RX + "income" + SPACE_RX + IDENT_RX +
         optional(SEP_RX + IDENT_RX) + END_RX),
      budgetLineReserveAmountRx(
         START_RX + SEP_RX + "reserve" + SPACE_RX + IDENT_RX +
         SEP_RX + CURR_RX + SPACE_RX + INTERVAL_RX +
         optional(SEP_RX + IDENT_RX) + END_RX),
      budgetLineReservePercentRx(
         START_RX + SEP_RX + "reserve" + SPACE_RX + IDENT_RX +
         SEP_RX + PERCENT_RX +
         optional(SEP_RX + IDENT_RX) + END_RX),
      budgetLineRoutineRx(
         START_RX + SEP_RX + "routine" + SPACE_RX + IDENT_RX +
         optional(SEP_RX + IDENT_RX) + END_RX),
      budgetLineWithholdingRx(
         START_RX + SEP_RX + "withholding" + SPACE_RX +
         IDENT_RX +
         optional(SEP_RX + IDENT_RX) + END_RX),
      commentRx(START_RX + NOTE_RX + END_RX),
      txnCompactRx(
         START_RX + DATE_RX + SPACE_RX + CLEAR_RX + SPACE_RX +
         IDENT_RX + SEP_RX + IDENT_RX + SEP_RX +
         IDENT_RX + SEP_RX + CURR_RX +
         optional(SPACE_RX + "=" + SPACE_RX + CURR_RX) +
         optional(SPACE_RX + NOTE_RX) + END_RX),
      txnCompactOffRx(
         START_RX + DATE_RX + SPACE_RX + CLEAR_RX + SPACE_RX +
         IDENT_RX + SEP_RX + IDENT_RX + SEP_RX +
         CURR_RX +
         optional(SPACE_RX + "=" + SPACE_RX + CURR_RX) +
         optional(SPACE_RX + NOTE_RX) + END_RX),
      txnRx(
         START_RX + DATE_RX + SPACE_RX + CLEAR_RX + SPACE_RX +
         IDENT_RX + SEP_RX + CURR_RX +
         optional(SPACE_RX + "=" + SPACE_RX + CURR_RX) +
         optional(SPACE_RX + NOTE_RX) + END_RX),
      txnLineRx(
         START_RX + SEP_RX + IDENT_RX + SEP_RX +
         IDENT_RX + SEP_RX + CURR_RX +
         optional(SPACE_RX + NOTE_RX) + END_RX),
      txnLineOffRx(
         START_RX + SEP_RX + IDENT_RX + SEP_RX +
         CURR_RX + optional(SPACE_RX + NOTE_RX) + END_RX)
   {
   }
};

static FileReaderRegEx* regEx = nullptr;

Currency FileReader::parseCurrency(std::string const& currency,
                                   std::string const& fileName, size_t lineNum)
{
   bool ok;
   Currency c(Currency::fromString(currency, &ok));
   if (!ok)
   {
      std::stringstream ss;
      ss << "Unable to parse currency '" << currency << "'";
      die(fileName, lineNum, ss.str());
   }
   return c;
}

Date FileReader::parseDate(std::string const& date, std::string const& dateFormat,
                           std::string const& fileName, size_t lineNum)
{
   Date d(Date::fromString(date, dateFormat));
   if (!d.isValid())
   {
      std::stringstream ss;
      ss << "Unable to parse date '" << date << "', expected something like '"
         << dateFormat << "'";
      die(fileName, lineNum, ss.str());
   }
   return d;
}

FileReader::FileReader(std::string const& fileName, Ledger& ledger) :
   m_fileName(fileName),
   m_ledger(ledger)
{
   regEx = new FileReaderRegEx;
}

void FileReader::readAll()
{
   m_file.open(m_fileName);
   if (!m_file)
   {
      std::stringstream ss;
      ss << "Unable to open input file '" << m_fileName << "'";
      die(ss.str());
   }
   while (hasLines())
   {
      std::string line(readLine());
      processLine(line);
   }
   m_file.close();
}

void FileReader::setDateFormat(std::string const& dateFormat)
{
   m_dateFormat = dateFormat;
}

void FileReader::processAccount(std::smatch const& match)
{
   std::shared_ptr<LedgerAccount> account(
            new LedgerAccount(m_fileName, m_lineNum));
   account->setDate(parseDate(match.str(1)));
   account->setMode(parseMode(match.str(2)));
   verifySetIdentifier(match[3], IdentifierType::ACCOUNT);
   account->setName(Identifier(match[3], Identifier::Type::ACCOUNT));
   m_ledger.appendItem(account);
}

void FileReader::processBlank()
{
   std::shared_ptr<LedgerBlank> blank(new LedgerBlank(m_fileName, m_lineNum));
   m_ledger.appendItem(blank);
}

void FileReader::processBudget(std::smatch& match)
{
   std::shared_ptr<LedgerBudget> budget(
            new LedgerBudget(m_fileName, m_lineNum));
   budget->setDate(parseDate(match.str(1)));
   budget->setInterval(parseInterval(match.str(2)));

   while (true)
   {
      std::string line(readLine());
      if (std::regex_match(line, match, regEx->budgetLineCancelRx))
      {
         auto entry = make_shared<LedgerBudgetCancelEntry>(
                         m_fileName, m_lineNum);
         verifyIdentifier(match[1], IdentifierType::CATEGORY);
         entry->setCategory(Identifier(match[1],
                                       Identifier::Type::CATEGORY));
         entry->setGoal(match[2]);
         budget->appendEntry(entry);
      }
      else if (std::regex_match(line, match, regEx->budgetLineCloseRx))
      {
         std::shared_ptr<LedgerBudgetCloseEntry> entry(
                  new LedgerBudgetCloseEntry(m_fileName, m_lineNum));
         verifyIdentifier(match[1], IdentifierType::CATEGORY);
         entry->setCategory(Identifier(match[1],
                                       Identifier::Type::CATEGORY));
         budget->appendEntry(entry);
      }
      else if (std::regex_match(line, match, regEx->budgetLineGoalRx))
      {
         auto entry = make_shared<LedgerBudgetGoalEntry>(m_fileName, m_lineNum);

         verifyIdentifier(match[1], IdentifierType::CATEGORY);
         entry->setCategory(Identifier(match[1], Identifier::Type::CATEGORY));
         entry->setGoal(match[2]);
         entry->setAmount(parseCurrency(match[3]));
         entry->setGoalDate(parseDate(match[4]));
         // TODO these do not have owners since they reference via their parent
         //   categories, but they inherit from something that provides them
         //  - then again, so does LedgerBudgetCloseEntry

         budget->appendEntry(entry);
      }
      else if (std::regex_match(line, match, regEx->budgetLineGoalsRx))
      {
         auto entry = make_shared<LedgerBudgetGoalsEntry>(
                         m_fileName, m_lineNum);
         verifySetIdentifier(match[1], IdentifierType::CATEGORY);
         entry->setCategory(Identifier(match.str(1),
                                       Identifier::Type::CATEGORY));
         verifySetIdentifier(match[2], IdentifierType::OWNER);
         entry->setOwner(Identifier(match.str(2),
                                    Identifier::Type::OWNER));
         budget->appendEntry(entry);
      }
      else if (std::regex_match(line, match, regEx->budgetLineIncomeRx))
      {
         std::shared_ptr<LedgerBudgetIncomeEntry> entry(
                  new LedgerBudgetIncomeEntry(m_fileName, m_lineNum));
         verifySetIdentifier(match[1], IdentifierType::CATEGORY);
         entry->setCategory(Identifier(match.str(1),
                                       Identifier::Type::CATEGORY));
         verifySetIdentifier(match[2], IdentifierType::OWNER);
         entry->setOwner(Identifier(match.str(2),
                                    Identifier::Type::OWNER));
         budget->appendEntry(entry);
      }
      else if (std::regex_match(line, match, regEx->budgetLineReserveAmountRx))
      {
         std::shared_ptr<LedgerBudgetReserveAmountEntry> entry(
                  new LedgerBudgetReserveAmountEntry(m_fileName, m_lineNum));
         verifySetIdentifier(match[1], IdentifierType::CATEGORY);
         entry->setCategory(Identifier(match.str(1),
                                       Identifier::Type::CATEGORY));
         verifySetIdentifier(match[4], IdentifierType::OWNER);
         entry->setOwner(Identifier(match.str(4),
                                    Identifier::Type::OWNER));
         entry->setAmount(parseCurrency(match.str(2)));
         entry->setInterval(parseInterval(match.str(3)));
         budget->appendEntry(entry);
      }
      else if (std::regex_match(line, match, regEx->budgetLineReservePercentRx))
      {
         std::shared_ptr<LedgerBudgetReservePercentEntry> entry(
                  new LedgerBudgetReservePercentEntry(m_fileName, m_lineNum));
         verifySetIdentifier(match[1], IdentifierType::CATEGORY);
         entry->setCategory(Identifier(match.str(1),
                                       Identifier::Type::CATEGORY));
         verifySetIdentifier(match[3], IdentifierType::OWNER);
         entry->setOwner(Identifier(match.str(3),
                                    Identifier::Type::OWNER));
         entry->setPercentage(std::stoul(match.str(2), nullptr, 10));
         budget->appendEntry(entry);
      }
      else if (std::regex_match(line, match, regEx->budgetLineRoutineRx))
      {
         std::shared_ptr<LedgerBudgetRoutineEntry> entry(
                  new LedgerBudgetRoutineEntry(m_fileName, m_lineNum));
         verifySetIdentifier(match[1], IdentifierType::CATEGORY);
         entry->setCategory(Identifier(match.str(1),
                                       Identifier::Type::CATEGORY));
         verifySetIdentifier(match[2], IdentifierType::OWNER);
         entry->setOwner(Identifier(match.str(2),
                                    Identifier::Type::OWNER));
         budget->appendEntry(entry);
      }
      else if (std::regex_match(line, match, regEx->budgetLineWithholdingRx))
      {
         std::shared_ptr<LedgerBudgetWithholdingEntry> entry(
                  new LedgerBudgetWithholdingEntry(m_fileName, m_lineNum));
         verifySetIdentifier(match[1], IdentifierType::CATEGORY);
         entry->setCategory(Identifier(match.str(1),
                                       Identifier::Type::CATEGORY));
         verifySetIdentifier(match[2], IdentifierType::OWNER);
         entry->setOwner(Identifier(match.str(2),
                                    Identifier::Type::OWNER));
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

void FileReader::processComment(std::smatch const& match)
{
   std::shared_ptr<LedgerComment> comment(
            new LedgerComment(m_fileName, m_lineNum));
   comment->setNote(match.str(1));
   m_ledger.appendItem(comment);
}

void FileReader::processCompactTransaction(std::smatch const& match)
{
   std::shared_ptr<LedgerTransaction> transaction(
            new LedgerTransaction(m_fileName, m_lineNum));
   verifyIdentifier(match[3], IdentifierType::ACCOUNT);
   transaction->setAccount(
            Identifier(match.str(3), Identifier::Type::ACCOUNT));
   if (match.str(7) != "")
   {
      transaction->setBalance(parseCurrency(match.str(7)));
   }

   bool ok;
   transaction->setStatus(
            transaction->statusFromString(match.str(2), &ok));
   if (!ok)
   {
      std::stringstream ss;
      ss << "Could not read transaction status '" << match.str(2) << "'";
      die(m_fileName, m_lineNum, ss.str());
   }

   transaction->setDate(parseDate(match.str(1)));
   if (match.str(8) != "")
   {
      transaction->setNote(match.str(8));
   }

   LedgerTransactionEntry entry;
   entry.setAmount(parseCurrency(match.str(6)));
   if (match.str(5)[0] == '@')
   {
      verifyIdentifier(match.str(5).substr(1), IdentifierType::OWNER);
      entry.setCategory(Identifier(match.str(5).substr(1),
                                   Identifier::Type::OWNER));
   }
   else
   {
      verifyIdentifier(match[5], IdentifierType::CATEGORY);
      entry.setCategory(Identifier(match.str(5),
                                   Identifier::Type::CATEGORY));
   }
   if (match.str(4)[0] == '@')
   {
      verifyIdentifier(match.str(4).substr(1), IdentifierType::ACCOUNT);
      entry.setPayee(Identifier(match.str(4).substr(1),
                                Identifier::Type::ACCOUNT));
   }
   else
   {
      entry.setPayee(Identifier(match.str(4),
                                Identifier::Type::GENERIC));
   }
   transaction->appendEntry(entry);

   m_ledger.appendItem(transaction);
}

void FileReader::processCompactTransactionOff(std::smatch const& match)
{
   std::shared_ptr<LedgerTransaction> transaction(
            new LedgerTransaction(m_fileName, m_lineNum));
   verifyIdentifier(match[3], IdentifierType::ACCOUNT);
   transaction->setAccount(
            Identifier(match.str(3), Identifier::Type::ACCOUNT));
   if (match.str(6) != "")
   {
      transaction->setBalance(parseCurrency(match.str(6)));
   }
   bool ok;
   transaction->setStatus(
            transaction->statusFromString(match.str(2), &ok));
   if (!ok)
   {
      std::stringstream ss;
      ss << "Could not read transaction status '" << match.str(2) << "'";
      die(m_fileName, m_lineNum, ss.str());
   }
   transaction->setDate(parseDate(match.str(1)));
   if (match.str(7) != "")
   {
      transaction->setNote(match.str(7));
   }

   LedgerTransactionEntry entry;
   entry.setAmount(parseCurrency(match.str(5)));
   if (match.str(4)[0] == '@')
   {
      verifyIdentifier(match.str(4).substr(1), IdentifierType::ACCOUNT);
      entry.setPayee(Identifier(match.str(4).substr(1),
                                Identifier::Type::ACCOUNT));
   }
   else
   {
      entry.setPayee(Identifier(match.str(4),
                                Identifier::Type::GENERIC));
   }
   transaction->appendEntry(entry);

   m_ledger.appendItem(transaction);
}

void FileReader::processLine(std::string const& line)
{
   std::smatch match;
   if (std::regex_match(line, match, regEx->accountRx))
   {
      processAccount(match);
   }
   else if (std::regex_match(line, match, regEx->budgetRx))
   {
      processBudget(match);
   }
   else if (std::regex_match(line, match, regEx->commentRx))
   {
      processComment(match);
   }
   else if (std::regex_match(line, match, regEx->txnCompactRx))
   {
      processCompactTransaction(match);
   }
   else if (std::regex_match(line, match, regEx->txnCompactOffRx))
   {
      processCompactTransactionOff(match);
   }
   else if (std::regex_match(line, match, regEx->txnRx))
   {
      processTransaction(match);
   }
   else if (line == "")
   {
      processBlank();
   }
   else
   {
      std::stringstream ss;
      ss << "Invalid contents '" << line << "'";
      die(m_fileName, m_lineNum, ss.str());
   }
}

void FileReader::processTransaction(std::smatch& match)
{
   std::shared_ptr<LedgerTransaction> xact(
            new LedgerTransaction(m_fileName, m_lineNum));
   verifyIdentifier(match[3], IdentifierType::ACCOUNT);
   xact->setAccount(
            Identifier(match.str(3), Identifier::Type::ACCOUNT));
   if (match.str(5) != "")
   {
      xact->setBalance(parseCurrency(match.str(5)));
   }
   bool ok;
   xact->setStatus(xact->statusFromString(match.str(2), &ok));
   if (!ok)
   {
      std::stringstream ss;
      ss << "Could not read transaction status '" << match.str(2) << "'";
      die(m_fileName, m_lineNum, ss.str());
   }
   xact->setDate(parseDate(match.str(1)));
   if (match.str(6) != "")
   {
      xact->setNote(match.str(6));
   }

   while (true)
   {
      std::string line(readLine());
      if (std::regex_match(line, match, regEx->txnLineRx))
      {
         LedgerTransactionEntry entry;
         entry.setAmount(parseCurrency(match.str(3)));
         if (match.str(2)[0] == '@')
         {
            verifyIdentifier(match.str(2).substr(1), IdentifierType::OWNER);
            entry.setCategory(Identifier(match.str(2).substr(1),
                                         Identifier::Type::OWNER));
         }
         else
         {
            verifyIdentifier(match[2], IdentifierType::CATEGORY);
            entry.setCategory(Identifier(match.str(2),
                                         Identifier::Type::CATEGORY));
         }
         if (match.str(4) != "")
         {
            entry.setNote(match.str(4));
         }
         if (match.str(1)[0] == '@')
         {
            verifyIdentifier(match.str(1).substr(1), IdentifierType::ACCOUNT);
            entry.setPayee(Identifier(match.str(1).substr(1),
                                      Identifier::Type::ACCOUNT));
         }
         else
         {
            entry.setPayee(Identifier(match.str(1),
                                      Identifier::Type::GENERIC));
         }
         xact->appendEntry(entry);
      }
      else if (std::regex_match(line, match, regEx->txnLineOffRx))
      {
         LedgerTransactionEntry entry;
         entry.setAmount(parseCurrency(match.str(2)));
         if (match.str(3) != "")
         {
            entry.setNote(match.str(3));
         }
         if (match.str(1)[0] == '@')
         {
            verifyIdentifier(match.str(1).substr(1), IdentifierType::ACCOUNT);
            entry.setPayee(Identifier(match.str(1).substr(1),
                                      Identifier::Type::ACCOUNT));
         }
         else
         {
            entry.setPayee(Identifier(match.str(1),
                                      Identifier::Type::GENERIC));
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
   return !m_lines.empty() || !m_file.eof();
}

std::string FileReader::readLine()
{
   if (!m_lines.empty())
   {
      std::string line(m_lines.top());
      m_lines.pop();
      ++m_lineNum;
      return line;
   }
   else if (m_file)
   {
      ++m_lineNum;
      std::string line;
      getline(m_file, line);
      return line;
   }
   else
   {
      return "";
   }
}

void FileReader::unReadLine(std::string const& line)
{
   --m_lineNum;
   m_lines.push(line);
}

FileReader::IdentifierType FileReader::identifierType(
      std::string const& identifier)
{
   if (!m_identifiers.count(identifier))
   {
      die(m_fileName, m_lineNum, "Unknown identifier '" + identifier + "'");
   }
   return m_identifiers[identifier];
}

Currency FileReader::parseCurrency(std::string const& currency)
{
   return parseCurrency(currency, m_fileName, m_lineNum);
}

Date FileReader::parseDate(std::string const& date)
{
   return parseDate(date, m_dateFormat, m_fileName, m_lineNum);
}

Interval FileReader::parseInterval(std::string const& interval)
{
   bool ok;
   Interval i(Interval::fromString(interval, &ok));
   if (!ok)
   {
      std::stringstream ss;
      ss << "Unable to parse interval '" << interval << "'";
      die(m_fileName, m_lineNum, ss.str());
   }
   return i;
}

LedgerAccount::Mode FileReader::parseMode(std::string const& mode)
{
   bool ok;
   LedgerAccount::Mode m(LedgerAccount::modeFromString(mode, &ok));
   if (!ok)
   {
      std::stringstream ss;
      ss << "Unknown account command '" << mode << "'";
      die(m_fileName, m_lineNum, ss.str());
   }
   return m;
}

void FileReader::verifyIdentifier(
      std::string const& identifier, IdentifierType type)
{
   if (identifierType(identifier) != type)
   {
      std::stringstream ss;
      switch (identifierType(identifier))
      {
         case IdentifierType::ACCOUNT:
            ss << "Account";
            break;
         case IdentifierType::CATEGORY:
            ss << "Budget category";
            break;
         case IdentifierType::OWNER:
            ss << "Budget category owner";
            break;
      }
      ss << " '" << identifier << "' now used as ";
      switch (type)
      {
         case IdentifierType::ACCOUNT:
            ss << "account";
            break;
         case IdentifierType::CATEGORY:
            ss << "budget category";
            break;
         case IdentifierType::OWNER:
            ss << "budget category owner";
            break;
      }
      die(m_fileName, m_lineNum, ss.str());
   }
}

void FileReader::verifySetIdentifier(
      std::string const& identifier, IdentifierType type)
{
   if (!m_identifiers.count(identifier))
   {
      m_identifiers[identifier] = type;
   }
   verifyIdentifier(identifier, type);
}
