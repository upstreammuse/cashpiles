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
using std::smatch;

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
   std::regex const accountBalanceRx;
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
   std::regex const txnCompactOffRx;
   std::regex const txn2Rx;
   std::regex const txn2LineRx;
   std::regex const txn2TrackingLineRx;

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
   account->setName(match[3]);
   verifySetIdentifier(account->name(), IdentifierType::ACCOUNT);
   m_ledger.appendItem(account);
}

void FileReader::processAccountBalance(std::smatch const& match)
{
   auto balance = make_shared<LedgerAccountBalance>(m_fileName, m_lineNum);
   balance->setDate(parseDate(match[1]));
   balance->setAccount(match[2]);
   verifySetIdentifier(balance->account(), IdentifierType::ACCOUNT);
   balance->setAmount(parseCurrency(match[3]));
   m_ledger.appendItem(balance);
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
         entry->setCategory(match[1]);
         verifyIdentifier(entry->category(), IdentifierType::CATEGORY);
         entry->setGoal(match[2]);
         budget->appendEntry(entry);
      }
      else if (std::regex_match(line, match, regEx->budgetLineCloseRx))
      {
         std::shared_ptr<LedgerBudgetCloseEntry> entry(
                  new LedgerBudgetCloseEntry(m_fileName, m_lineNum));
         entry->setCategory(match[1]);
         verifyIdentifier(entry->category(), IdentifierType::CATEGORY);
         budget->appendEntry(entry);
      }
      else if (std::regex_match(line, match, regEx->budgetLineGoalRx))
      {
         auto entry = make_shared<LedgerBudgetGoalEntry>(m_fileName, m_lineNum);

         entry->setCategory(match[1]);
         verifyIdentifier(entry->category(), IdentifierType::CATEGORY);
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
         entry->setCategory(match[1]);
         verifySetIdentifier(entry->category(), IdentifierType::CATEGORY);
         entry->setOwner(match[2]);
         verifySetIdentifier(entry->owner(), IdentifierType::OWNER);
         budget->appendEntry(entry);
      }
      else if (std::regex_match(line, match, regEx->budgetLineIncomeRx))
      {
         std::shared_ptr<LedgerBudgetIncomeEntry> entry(
                  new LedgerBudgetIncomeEntry(m_fileName, m_lineNum));
         entry->setCategory(match[1]);
         verifySetIdentifier(entry->category(), IdentifierType::CATEGORY);
         entry->setOwner(match[2]);
         verifySetIdentifier(entry->owner(), IdentifierType::OWNER);
         budget->appendEntry(entry);
      }
      else if (std::regex_match(line, match, regEx->budgetLineReserveAmountRx))
      {
         std::shared_ptr<LedgerBudgetReserveAmountEntry> entry(
                  new LedgerBudgetReserveAmountEntry(m_fileName, m_lineNum));
         entry->setCategory(match[1]);
         verifySetIdentifier(entry->category(), IdentifierType::CATEGORY);
         entry->setAmount(parseCurrency(match[2]));
         entry->setInterval(parseInterval(match[3]));
         entry->setOwner(match[4]);
         verifySetIdentifier(entry->owner(), IdentifierType::OWNER);
         budget->appendEntry(entry);
      }
      else if (std::regex_match(line, match, regEx->budgetLineReservePercentRx))
      {
         std::shared_ptr<LedgerBudgetReservePercentEntry> entry(
                  new LedgerBudgetReservePercentEntry(m_fileName, m_lineNum));
         entry->setCategory(match[1]);
         verifySetIdentifier(entry->category(), IdentifierType::CATEGORY);
         entry->setPercentage(std::stoul(match.str(2), nullptr, 10));
         entry->setOwner(match[3]);
         verifySetIdentifier(entry->owner(), IdentifierType::OWNER);
         budget->appendEntry(entry);
      }
      else if (std::regex_match(line, match, regEx->budgetLineRoutineRx))
      {
         std::shared_ptr<LedgerBudgetRoutineEntry> entry(
                  new LedgerBudgetRoutineEntry(m_fileName, m_lineNum));
         entry->setCategory(match[1]);
         verifySetIdentifier(entry->category(), IdentifierType::CATEGORY);
         entry->setOwner(match[2]);
         verifySetIdentifier(entry->owner(), IdentifierType::OWNER);
         budget->appendEntry(entry);
      }
      else if (std::regex_match(line, match, regEx->budgetLineWithholdingRx))
      {
         std::shared_ptr<LedgerBudgetWithholdingEntry> entry(
                  new LedgerBudgetWithholdingEntry(m_fileName, m_lineNum));
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

void FileReader::processComment(std::smatch const& match)
{
   std::shared_ptr<LedgerComment> comment(
            new LedgerComment(m_fileName, m_lineNum));
   comment->setNote(match.str(1));
   m_ledger.appendItem(comment);
}

void FileReader::processCompactTransactionOff(std::smatch const& match)
{
   std::shared_ptr<LedgerTransaction> transaction(
            new LedgerTransaction(m_fileName, m_lineNum));
   transaction->setDate(parseDate(match.str(1)));
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
      transaction->setNote(match.str(6));
   }
   m_ledger.appendItem(transaction);
}

void FileReader::processLine(std::string const& line)
{
   std::smatch match;
   if (std::regex_match(line, match, regEx->accountRx))
   {
      processAccount(match);
   }
   else if (std::regex_match(line, match, regEx->accountBalanceRx))
   {
      processAccountBalance(match);
   }
   else if (std::regex_match(line, match, regEx->budgetRx))
   {
      processBudget(match);
   }
   else if (std::regex_match(line, match, regEx->commentRx))
   {
      processComment(match);
   }
   else if (std::regex_match(line, match, regEx->txnCompactOffRx))
   {
      processCompactTransactionOff(match);
   }
   else if (std::regex_match(line, match, regEx->txn2Rx))
   {
      processTransactionV2(match);
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

void FileReader::processTransactionV2(smatch& match)
{
   auto txn = make_shared<LedgerTransactionV2>(m_fileName, m_lineNum);
   txn->setDate(parseDate(match[1]));
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
