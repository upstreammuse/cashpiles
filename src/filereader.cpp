#include "filereader.h"

#include <regex>
#include <sstream>
#include "cashpiles.h"
#include "date.h"
#include "ledger.h"
#include "ledgeraccount.h"
#include "ledgeraccountbalance.h"
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
#include "ledgertransactionv2.h"

using std::make_shared;
using std::regex;
using std::regex_match;
using std::smatch;
using std::string;
using std::stringstream;

struct FileReaderRegEx
{
   string const CLEAR_RX;
   string const CURR_RX;
   string const DATE_RX;
   string const END_RX;
   string const IDENT_RX;
   string const INTERVAL_RX;
   string const NOTE_RX;
   string const PERCENT_RX;
   string const SEP_RX;
   string const SPACE_RX;
   string const START_RX;

   regex const accountRx;
   regex const accountBalanceRx;
   regex const budgetRx;
   regex const budgetLineCancelRx;
   regex const budgetLineCloseRx;
   regex const budgetLineGoalRx;
   regex const budgetLineGoalsRx;
   regex const budgetLineIncomeRx;
   regex const budgetLineReserveAmountRx;
   regex const budgetLineReservePercentRx;
   regex const budgetLineRoutineRx;
   regex const budgetLineWithholdingRx;
   regex const commentRx;
   regex const txnCompactOffRx;
   regex const txn2Rx;
   regex const txn2LineRx;
   regex const txn2TrackingLineRx;

   string currencyRx()
   {
      stringstream retval;
      struct lconv* lc = localeconv();
      retval << "((?:";

      string symbol(lc->currency_symbol);
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

      string sep(lc->mon_thousands_sep);
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

      string decimal(lc->mon_decimal_point);
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

      string negative(lc->negative_sign);
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

   string optional(string const& item)
   {
      stringstream ss;
      ss << "(?:" << item << ")?";
      return ss.str();
   }

   // TODO proposal for ledgeritem to contain the note, since the data model aligns to the file such that each line is its own item, and the file reader can play 'find the semicolon' to split the note away from the rest of the content for regex parsing.  way better than each item child having to deal with note processing on its own
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
      accountBalanceRx(
         START_RX + DATE_RX + SPACE_RX + "balance" + SPACE_RX + IDENT_RX +
         SEP_RX + CURR_RX + END_RX),
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
      txnCompactOffRx(
         START_RX + DATE_RX + SPACE_RX + CLEAR_RX + SPACE_RX + IDENT_RX +
         SEP_RX + IDENT_RX + SEP_RX + CURR_RX + optional(SPACE_RX + NOTE_RX) +
         END_RX),
      txn2Rx(
         START_RX + DATE_RX + SPACE_RX + CLEAR_RX + SPACE_RX + IDENT_RX +
         optional(SEP_RX + CURR_RX) + optional(SPACE_RX + NOTE_RX)),
      txn2LineRx(
         START_RX + SEP_RX + IDENT_RX + optional(SEP_RX + CURR_RX) +
         optional(SPACE_RX + NOTE_RX)),
      txn2TrackingLineRx(
         START_RX + SEP_RX + IDENT_RX + SEP_RX + IDENT_RX +
         optional(SEP_RX + CURR_RX) + optional(SPACE_RX + NOTE_RX))
   {
   }
};

static FileReaderRegEx* regEx = nullptr;

Currency FileReader::parseCurrency(
      string const& currency, string const& fileName, size_t lineNum)
{
   bool ok;
   Currency c(Currency::fromString(currency, &ok));
   if (!ok)
   {
      stringstream ss;
      ss << "Unable to parse currency '" << currency << "'";
      die(fileName, lineNum, ss.str());
   }
   return c;
}

Date FileReader::parseDate(
      string const& date, string const& dateFormat, string const& fileName,
      size_t lineNum)
{
   try
   {
      return Date::fromString(date, dateFormat);
   }
   catch (...)
   {
      stringstream ss;
      ss << "Unable to parse date '" << date << "', expected something like '"
         << dateFormat << "'";
      die(fileName, lineNum, ss.str());
   }
}

FileReader::FileReader(string const& fileName, Ledger& ledger) :
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
      stringstream ss;
      ss << "Unable to open input file '" << m_fileName << "'";
      die(ss.str());
   }
   while (hasLines())
   {
      string line(readLine());
      processLine(line);
   }
   m_file.close();
}

void FileReader::setDateFormat(string const& dateFormat)
{
   m_dateFormat = dateFormat;
}

void FileReader::processAccount(smatch const& match)
{
   auto account = make_shared<LedgerAccount>(m_fileName, m_lineNum);
   account->setDate(parseDate(match[1]));
   account->setMode(parseMode(match[2]));
   account->setName(match[3]);
   verifySetIdentifier(account->name(), IdentifierType::ACCOUNT);
   m_ledger.appendItem(account);
}

void FileReader::processAccountBalance(smatch const& match)
{
   auto balance = make_shared<LedgerAccountBalance>(parseDate(match[1]), m_fileName, m_lineNum);
   balance->setAccount(match[2]);
   verifySetIdentifier(balance->account(), IdentifierType::ACCOUNT);
   balance->setAmount(parseCurrency(match[3]));
   m_ledger.appendItem(balance);
}

void FileReader::processBlank()
{
   auto blank = make_shared<LedgerBlank>(m_fileName, m_lineNum);
   m_ledger.appendItem(blank);
}

void FileReader::processBudget(smatch& match)
{
   auto budget = make_shared<LedgerBudget>(m_fileName, m_lineNum);
   budget->setDate(parseDate(match[1]));
   budget->setInterval(parseInterval(match[2]));

   while (true)
   {
      auto line = readLine();
      if (regex_match(line, match, regEx->budgetLineCancelRx))
      {
         auto entry = make_shared<LedgerBudgetCancelEntry>(
                         m_fileName, m_lineNum);
         entry->setCategory(match[1]);
         verifyIdentifier(entry->category(), IdentifierType::CATEGORY);
         entry->setGoal(match[2]);
         budget->appendEntry(entry);
      }
      else if (regex_match(line, match, regEx->budgetLineCloseRx))
      {
         auto entry = make_shared<LedgerBudgetCloseEntry>(
                         m_fileName, m_lineNum);
         entry->setCategory(match[1]);
         verifyIdentifier(entry->category(), IdentifierType::CATEGORY);
         budget->appendEntry(entry);
      }
      else if (regex_match(line, match, regEx->budgetLineGoalRx))
      {
         auto entry = make_shared<LedgerBudgetGoalEntry>(parseDate(match[4]), m_fileName, m_lineNum);
         entry->setCategory(match[1]);
         verifyIdentifier(entry->category(), IdentifierType::CATEGORY);
         entry->setGoal(match[2]);
         entry->setAmount(parseCurrency(match[3]));
         // TODO these do not have owners since they reference via their parent
         //   categories, but they inherit from something that provides them
         //  - then again, so does LedgerBudgetCloseEntry
         budget->appendEntry(entry);
      }
      else if (regex_match(line, match, regEx->budgetLineGoalsRx))
      {
         auto entry = make_shared<LedgerBudgetGoalsEntry>(
                         m_fileName, m_lineNum);
         entry->setCategory(match[1]);
         verifySetIdentifier(entry->category(), IdentifierType::CATEGORY);
         entry->setOwner(match[2]);
         verifySetIdentifier(entry->owner(), IdentifierType::OWNER);
         budget->appendEntry(entry);
      }
      else if (regex_match(line, match, regEx->budgetLineIncomeRx))
      {
         auto entry = make_shared<LedgerBudgetIncomeEntry>(
                         m_fileName, m_lineNum);
         entry->setCategory(match[1]);
         verifySetIdentifier(entry->category(), IdentifierType::CATEGORY);
         entry->setOwner(match[2]);
         verifySetIdentifier(entry->owner(), IdentifierType::OWNER);
         budget->appendEntry(entry);
      }
      else if (regex_match(line, match, regEx->budgetLineReserveAmountRx))
      {
         auto entry = make_shared<LedgerBudgetReserveAmountEntry>(
                         m_fileName, m_lineNum);
         entry->setCategory(match[1]);
         verifySetIdentifier(entry->category(), IdentifierType::CATEGORY);
         entry->setAmount(parseCurrency(match[2]));
         entry->setInterval(parseInterval(match[3]));
         entry->setOwner(match[4]);
         verifySetIdentifier(entry->owner(), IdentifierType::OWNER);
         budget->appendEntry(entry);
      }
      else if (regex_match(line, match, regEx->budgetLineReservePercentRx))
      {
         auto entry = make_shared<LedgerBudgetReservePercentEntry>(
                         m_fileName, m_lineNum);
         entry->setCategory(match[1]);
         verifySetIdentifier(entry->category(), IdentifierType::CATEGORY);
         entry->setPercentage(stoul(match.str(2), nullptr, 10));
         entry->setOwner(match[3]);
         verifySetIdentifier(entry->owner(), IdentifierType::OWNER);
         budget->appendEntry(entry);
      }
      else if (regex_match(line, match, regEx->budgetLineRoutineRx))
      {
         auto entry = make_shared<LedgerBudgetRoutineEntry>(
                         m_fileName, m_lineNum);
         entry->setCategory(match[1]);
         verifySetIdentifier(entry->category(), IdentifierType::CATEGORY);
         entry->setOwner(match[2]);
         verifySetIdentifier(entry->owner(), IdentifierType::OWNER);
         budget->appendEntry(entry);
      }
      else if (regex_match(line, match, regEx->budgetLineWithholdingRx))
      {
         auto entry = make_shared<LedgerBudgetWithholdingEntry>(
                         m_fileName, m_lineNum);
         entry->setCategory(match[1]);
         verifySetIdentifier(entry->category(), IdentifierType::CATEGORY);
         entry->setOwner(match[2]);
         verifySetIdentifier(entry->owner(), IdentifierType::OWNER);
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

void FileReader::processComment(smatch const& match)
{
   auto comment = make_shared<LedgerComment>(m_fileName, m_lineNum);
   comment->setNote(match.str(1));
   m_ledger.appendItem(comment);
}

void FileReader::processCompactTransactionOff(smatch const& match)
{
   auto transaction = make_shared<LedgerTransaction>(m_fileName, m_lineNum);
   transaction->setDate(parseDate(match[1]));
   switch (match.str(2)[0])
   {
      case '*':
         transaction->setStatus(LedgerTransaction::Status::CLEARED);
         break;
      case '!':
         transaction->setStatus(LedgerTransaction::Status::DISPUTED);
         break;
      case '?':
         transaction->setStatus(LedgerTransaction::Status::PENDING);
         break;
   }
   transaction->setAccount(match[3]);
   verifyIdentifier(transaction->account(), IdentifierType::ACCOUNT);
   transaction->setPayee(match[4]);
   transaction->setAmount(parseCurrency(match[5]));
   if (match[6] != "")
   {
      transaction->setNote(match[6]);
   }
   m_ledger.appendItem(transaction);
}

void FileReader::processLine(string const& line)
{
   smatch match;
   if (regex_match(line, match, regEx->accountRx))
   {
      processAccount(match);
   }
   else if (regex_match(line, match, regEx->accountBalanceRx))
   {
      processAccountBalance(match);
   }
   else if (regex_match(line, match, regEx->budgetRx))
   {
      processBudget(match);
   }
   else if (regex_match(line, match, regEx->commentRx))
   {
      processComment(match);
   }
   else if (regex_match(line, match, regEx->txnCompactOffRx))
   {
      processCompactTransactionOff(match);
   }
   else if (regex_match(line, match, regEx->txn2Rx))
   {
      processTransactionV2(match);
   }
   else if (line == "")
   {
      processBlank();
   }
   else
   {
      stringstream ss;
      ss << "Invalid contents '" << line << "'";
      die(m_fileName, m_lineNum, ss.str());
   }
}

void FileReader::processTransactionV2(smatch& match)
{
   auto txn = make_shared<LedgerTransactionV2>(parseDate(match[1]), m_fileName, m_lineNum);
   auto status = match.str(2)[0];
   switch (status)
   {
      case '*':
         txn->setStatus(LedgerTransactionV2::Status::CLEARED);
         break;
      case '!':
         txn->setStatus(LedgerTransactionV2::Status::DISPUTED);
         break;
      case '?':
         txn->setStatus(LedgerTransactionV2::Status::PENDING);
         break;
   }
   txn->setPayee(match[3]);
   if (match[5] != "")
   {
      txn->setNote(match[5]);
   }

   while (true)
   {
      auto line = readLine();
      if (regex_match(line, match, regEx->txn2LineRx))
      {
         switch (identifierType(match[1]))
         {
            case IdentifierType::ACCOUNT:
            {
               auto entry = make_shared<LedgerTransactionV2AccountEntry>(
                               m_fileName, m_lineNum);
               entry->setAccount(match[1]);
               if (match[2] != "")
               {
                  entry->setAmount(parseCurrency(match[2]));
               }
               if (match[3] != "")
               {
                  entry->setNote(match[3]);
               }
               txn->appendEntry(entry);
               break;
            }
            case IdentifierType::CATEGORY:
            {
               auto entry = make_shared<LedgerTransactionV2CategoryEntry>(
                               m_fileName, m_lineNum);
               entry->setCategory(match[1]);
               if (match[2] != "")
               {
                  entry->setAmount(parseCurrency(match[2]));
               }
               if (match[3] != "")
               {
                  entry->setNote(match[3]);
               }
               txn->appendEntry(entry);
               break;
            }
            case IdentifierType::OWNER:
            {
               auto entry = make_shared<LedgerTransactionV2OwnerEntry>(
                               m_fileName, m_lineNum);
               entry->setOwner(match[1]);
               if (match[2] != "")
               {
                  entry->setAmount(parseCurrency(match[2]));
               }
               if (match[3] != "")
               {
                  entry->setNote(match[3]);
               }
               txn->appendEntry(entry);
               break;
            }
         }
      }
      else if (regex_match(line, match, regEx->txn2TrackingLineRx))
      {
         switch (identifierType(match[1]))
         {
            case IdentifierType::ACCOUNT:
            {
               die(m_fileName, m_lineNum,
                   "Cannot use tracking account with account");
               // TODO put a break here when die() is removed
            }
            case IdentifierType::CATEGORY:
            {
               auto entry = make_shared<LedgerTransactionV2CategoryEntry>(
                               m_fileName, m_lineNum);
               entry->setCategory(match[1]);
               entry->setTrackingAccount(match[2]);
               verifyIdentifier(entry->trackingAccount().first,
                                IdentifierType::ACCOUNT);
               if (match[3] != "")
               {
                  entry->setAmount(parseCurrency(match[3]));
               }
               if (match[4] != "")
               {
                  entry->setNote(match[4]);
               }
               txn->appendEntry(entry);
               break;
            }
            case IdentifierType::OWNER:
            {
               auto entry = make_shared<LedgerTransactionV2OwnerEntry>(
                               m_fileName, m_lineNum);
               entry->setOwner(match[1]);
               entry->setTrackingAccount(match[2]);
               verifyIdentifier(entry->trackingAccount().first,
                                IdentifierType::ACCOUNT);
               if (match[3] != "")
               {
                  entry->setAmount(parseCurrency(match[3]));
               }
               if (match[4] != "")
               {
                  entry->setNote(match[4]);
               }
               txn->appendEntry(entry);
               break;
            }
         }
      }
      else
      {
         unReadLine(line);
         break;
      }
   }

   txn->finalize();
   m_ledger.appendItem(txn);
}

bool FileReader::hasLines()
{
   return !m_lines.empty() || !m_file.eof();
}

string FileReader::readLine()
{
   if (!m_lines.empty())
   {
      string line(m_lines.top());
      m_lines.pop();
      ++m_lineNum;
      return line;
   }
   else if (m_file)
   {
      ++m_lineNum;
      string line;
      getline(m_file, line);
      return line;
   }
   else
   {
      return "";
   }
}

void FileReader::unReadLine(string const& line)
{
   --m_lineNum;
   m_lines.push(line);
}

FileReader::IdentifierType FileReader::identifierType(string const& identifier)
{
   if (!m_identifiers.count(identifier))
   {
      die(m_fileName, m_lineNum, "Unknown identifier '" + identifier + "'");
   }
   return m_identifiers[identifier];
}

Currency FileReader::parseCurrency(string const& currency)
{
   return parseCurrency(currency, m_fileName, m_lineNum);
}

Date FileReader::parseDate(string const& date)
{
   return parseDate(date, m_dateFormat, m_fileName, m_lineNum);
}

Interval FileReader::parseInterval(string const& interval)
{
   bool ok;
   Interval i(Interval::fromString(interval, &ok));
   if (!ok)
   {
      stringstream ss;
      ss << "Unable to parse interval '" << interval << "'";
      die(m_fileName, m_lineNum, ss.str());
   }
   return i;
}

LedgerAccount::Mode FileReader::parseMode(string const& mode)
{
   bool ok;
   LedgerAccount::Mode m(LedgerAccount::modeFromString(mode, &ok));
   if (!ok)
   {
      stringstream ss;
      ss << "Unknown account command '" << mode << "'";
      die(m_fileName, m_lineNum, ss.str());
   }
   return m;
}

void FileReader::verifyIdentifier(string const& identifier, IdentifierType type)
{
   if (identifierType(identifier) != type)
   {
      stringstream ss;
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
      string const& identifier, IdentifierType type)
{
   if (!m_identifiers.count(identifier))
   {
      m_identifiers[identifier] = type;
   }
   verifyIdentifier(identifier, type);
}
