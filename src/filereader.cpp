#include "filereader.h"

#include <cassert>
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
#include "ledgertransaction.h"
#include "ledgertransactionv2.h"

using std::make_shared;
using std::regex;
using std::regex_match;
using std::smatch;
using std::string;
using std::stringstream;

FileReader::FileReader()
{
}

void FileReader::readAll(Ledger& ledger, string const& fileName)
{
   m_identifiers.clear();
   m_lineNum = 0;
   while (!m_lines.empty())
   {
      m_lines.pop();
   }

   std::ifstream file(fileName);
   if (!file)
   {
      stringstream ss;
      ss << "Unable to open input file '" << m_fileName << "'";
      die(ss.str());
   }
   while (hasLines(file))
   {
      string line(readLine(file));
      processLine(ledger, line);
   }
   file.close();
}

void FileReader::setDateFormat(string const& dateFormat)
{
   m_dateFormat = dateFormat;
}

void FileReader::processAccount(Ledger& ledger, smatch const& match)
{
   auto account = make_shared<LedgerAccount>(
                     parseDate(match[1]), *m_activeItem);
   account->setMode(parseMode(match[2]));
   account->setName(match[3]);
   verifySetIdentifier(account->name(), IdentifierType::ACCOUNT);
   ledger.appendItem(account);
}

void FileReader::processAccountBalance(Ledger& ledger, smatch const& match)
{
   auto balance = make_shared<LedgerAccountBalance>(
                     parseDate(match[1]), *m_activeItem);
   balance->setAccount(match[2]);
   verifySetIdentifier(balance->account(), IdentifierType::ACCOUNT);
   balance->setAmount(parseCurrency(match[3]));
   ledger.appendItem(balance);
}

void FileReader::processBlank(Ledger& ledger)
{
   auto blank = make_shared<LedgerBlank>(*m_activeItem);
   ledger.appendItem(blank);
}

void FileReader::processBudget(smatch& match)
{
   auto date = parseDate(match[1]);
   auto interval = parseInterval(match[2]);
   assert(!m_activeBudget);
   m_activeBudget = make_shared<LedgerBudget>(date, interval, *m_activeItem);
}

void FileReader::processBudgetCancel(smatch& match)
{
   auto entry = make_shared<LedgerBudgetCancelEntry>(*m_activeItem);
   entry->setCategory(match[1]);
   verifyIdentifier(entry->category(), IdentifierType::CATEGORY);
   entry->setGoal(match[2]);
   m_activeBudget->appendEntry(entry);
}

void FileReader::processBudgetClose(smatch& match)
{
   auto entry = make_shared<LedgerBudgetCloseEntry>(*m_activeItem);
   entry->setCategory(match[1]);
   verifyIdentifier(entry->category(), IdentifierType::CATEGORY);
   m_activeBudget->appendEntry(entry);
}

void FileReader::processBudgetGoal(smatch& match)
{
   auto entry = make_shared<LedgerBudgetGoalEntry>(
                   parseDate(match[4]), *m_activeItem);
   entry->setCategory(match[1]);
   verifyIdentifier(entry->category(), IdentifierType::CATEGORY);
   entry->setGoal(match[2]);
   entry->setAmount(parseCurrency(match[3]));
   // TODO these do not have owners since they reference via their parent
   //   categories, but they inherit from something that provides them
   //  - then again, so does LedgerBudgetCloseEntry (and cancel)
   m_activeBudget->appendEntry(entry);
}

void FileReader::processBudgetGoals(smatch& match)
{
   auto entry = make_shared<LedgerBudgetGoalsEntry>(*m_activeItem);
   entry->setCategory(match[1]);
   verifySetIdentifier(entry->category(), IdentifierType::CATEGORY);
   entry->setOwner(match[2]);
   verifySetIdentifier(entry->owner(), IdentifierType::OWNER);
   m_activeBudget->appendEntry(entry);
}

void FileReader::processBudgetIncome(smatch& match)
{
   auto entry = make_shared<LedgerBudgetIncomeEntry>(*m_activeItem);
   entry->setCategory(match[1]);
   verifySetIdentifier(entry->category(), IdentifierType::CATEGORY);
   entry->setOwner(match[2]);
   verifySetIdentifier(entry->owner(), IdentifierType::OWNER);
   m_activeBudget->appendEntry(entry);
}

void FileReader::processBudgetReserveAmount(smatch& match)
{
   auto interval = parseInterval(match[3]);
   auto entry = make_shared<LedgerBudgetReserveAmountEntry>(
                   interval, *m_activeItem);
   entry->setCategory(match[1]);
   verifySetIdentifier(entry->category(), IdentifierType::CATEGORY);
   entry->setAmount(parseCurrency(match[2]));
   entry->setOwner(match[4]);
   verifySetIdentifier(entry->owner(), IdentifierType::OWNER);
   m_activeBudget->appendEntry(entry);
}

void FileReader::processBudgetReservePercent(smatch& match)
{
   auto entry = make_shared<LedgerBudgetReservePercentEntry>(*m_activeItem);
   entry->setCategory(match[1]);
   verifySetIdentifier(entry->category(), IdentifierType::CATEGORY);
   entry->setPercentage(stoul(match.str(2), nullptr, 10));
   entry->setOwner(match[3]);
   verifySetIdentifier(entry->owner(), IdentifierType::OWNER);
   m_activeBudget->appendEntry(entry);
}

void FileReader::processBudgetRoutine(smatch& match)
{
   auto entry = make_shared<LedgerBudgetRoutineEntry>(*m_activeItem);
   entry->setCategory(match[1]);
   verifySetIdentifier(entry->category(), IdentifierType::CATEGORY);
   entry->setOwner(match[2]);
   verifySetIdentifier(entry->owner(), IdentifierType::OWNER);
   m_activeBudget->appendEntry(entry);
}

void FileReader::processBudgetWithholding(smatch& match)
{
   auto entry = make_shared<LedgerBudgetWithholdingEntry>(*m_activeItem);
   entry->setCategory(match[1]);
   verifySetIdentifier(entry->category(), IdentifierType::CATEGORY);
   entry->setOwner(match[2]);
   verifySetIdentifier(entry->owner(), IdentifierType::OWNER);
   m_activeBudget->appendEntry(entry);
}

void FileReader::processCompactTransactionOff(
      Ledger& ledger, smatch const& match)
{
   auto transaction = make_shared<LedgerTransaction>(
                         parseDate(match[1]), *m_activeItem);
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
   ledger.appendItem(transaction);
}

void FileReader::processLine(Ledger& ledger, string const& fullLine)
{
   // Based on what I can find about the C++ regex spec, this is the regex that
   // will split the line into the line and the comment.  However, it appears to
   // be broken and failing to match.  I've debugged with online regex tools to
   // verify the logic, so I'm at a loss to explain why it doesn't work.  Going
   // to stick with basic string manipulation that I can trust, since the format
   // is easy enough.

   // strings without ';' that end in a non-space character, possible space,
   // then an optional comment starting with ';' and ending with a non-space
   // character, possible space
//   auto lineRx = regex {"^([^;]*[^;[:s:]])?\\s*(?:;(.*\\S))?\\s*$"};

   m_activeItem = make_shared<LedgerBlank>(m_fileName, m_lineNum);
   auto loc = fullLine.find(';');
   string comment;
   string line;
   if (loc != string::npos)
   {
      comment = fullLine.substr(loc + 1);
      line = fullLine.substr(0, loc);
   }
   while (isspace(line.back()))
   {
      line = line.substr(0, line.size() - 1);
   }
   while (isspace(comment.back()))
   {
      comment = comment.substr(0, comment.size() - 1);
   }
   if (comment != "")
   {
      m_activeItem->setNote(comment);
   }

   smatch match;
   if (m_activeBudget)
   {
      if (regex_match(line, match, m_regEx.budgetLineCancelRx))
      {
         processBudgetCancel(match);
      }
      else if (regex_match(line, match, m_regEx.budgetLineCloseRx))
      {
         processBudgetClose(match);
      }
      else if (regex_match(line, match, m_regEx.budgetLineGoalRx))
      {
         processBudgetGoal(match);
      }
      else if (regex_match(line, match, m_regEx.budgetLineGoalsRx))
      {
         processBudgetGoals(match);
      }
      else if (regex_match(line, match, m_regEx.budgetLineIncomeRx))
      {
         processBudgetIncome(match);
      }
      else if (regex_match(line, match, m_regEx.budgetLineReserveAmountRx))
      {
         processBudgetReserveAmount(match);
      }
      else if (regex_match(line, match, m_regEx.budgetLineReservePercentRx))
      {
         processBudgetReservePercent(match);
      }
      else if (regex_match(line, match, m_regEx.budgetLineRoutineRx))
      {
         processBudgetRoutine(match);
      }
      else if (regex_match(line, match, m_regEx.budgetLineWithholdingRx))
      {
         processBudgetWithholding(match);
      }
      else
      {
         unReadLine(fullLine);
         ledger.appendItem(m_activeBudget);
         m_activeBudget.reset();
      }
   }
   else if (m_activeTransaction)
   {
      if (regex_match(line, match, m_regEx.txn2LineRx))
      {
         processTransactionV2Line(match);
      }
      else if (regex_match(line, match, m_regEx.txn2TrackingLineRx))
      {
         processTransactionV2TrackingLine(match);
      }
      else
      {
         unReadLine(fullLine);
         m_activeTransaction->finalize();
         ledger.appendItem(m_activeTransaction);
         m_activeTransaction.reset();
      }
   }
   else if (regex_match(line, match, m_regEx.accountRx))
   {
      processAccount(ledger, match);
   }
   else if (regex_match(line, match, m_regEx.accountBalanceRx))
   {
      processAccountBalance(ledger, match);
   }
   else if (regex_match(line, match, m_regEx.budgetRx))
   {
      processBudget(match);
   }
   else if (regex_match(line, match, m_regEx.txnCompactOffRx))
   {
      processCompactTransactionOff(ledger, match);
   }
   else if (regex_match(line, match, m_regEx.txn2Rx))
   {
      processTransactionV2(match);
   }
   else if (line == "")
   {
      processBlank(ledger);
   }
   else
   {
      stringstream ss;
      ss << "Invalid contents '" << fullLine << "'";
      die(m_fileName, m_lineNum, ss.str());
   }
}

void FileReader::processTransactionV2(smatch& match)
{
   assert(!m_activeTransaction);
   m_activeTransaction = make_shared<LedgerTransactionV2>(
                            parseDate(match[1]), *m_activeItem);
   auto status = match.str(2)[0];
   switch (status)
   {
      case '*':
         m_activeTransaction->setStatus(LedgerTransactionV2::Status::CLEARED);
         break;
      case '!':
         m_activeTransaction->setStatus(LedgerTransactionV2::Status::DISPUTED);
         break;
      case '?':
         m_activeTransaction->setStatus(LedgerTransactionV2::Status::PENDING);
         break;
   }
   m_activeTransaction->setPayee(match[3]);
}

void FileReader::processTransactionV2Line(smatch& match)
{
   switch (identifierType(match[1]))
   {
      case IdentifierType::ACCOUNT:
      {
         auto entry = make_shared<LedgerTransactionV2AccountEntry>(
                         *m_activeItem);
         entry->setAccount(match[1]);
         verifyIdentifier(entry->account(), IdentifierType::ACCOUNT);
         if (match[2] != "")
         {
            entry->setAmount(parseCurrency(match[2]));
         }
         m_activeTransaction->appendEntry(entry);
         break;
      }
      case IdentifierType::CATEGORY:
      {
         auto entry = make_shared<LedgerTransactionV2CategoryEntry>(
                         *m_activeItem);
         entry->setCategory(match[1]);
         verifyIdentifier(entry->category(), IdentifierType::CATEGORY);
         if (match[2] != "")
         {
            entry->setAmount(parseCurrency(match[2]));
         }
         m_activeTransaction->appendEntry(entry);
         break;
      }
      case IdentifierType::OWNER:
      {
         auto entry = make_shared<LedgerTransactionV2OwnerEntry>(*m_activeItem);
         entry->setOwner(match[1]);
         verifyIdentifier(entry->owner(), IdentifierType::OWNER);
         if (match[2] != "")
         {
            entry->setAmount(parseCurrency(match[2]));
         }
         m_activeTransaction->appendEntry(entry);
         break;
      }
   }
}

void FileReader::processTransactionV2TrackingLine(smatch& match)
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
                         *m_activeItem);
         entry->setCategory(match[1]);
         entry->setTrackingAccount(match[2]);
         verifyIdentifier(entry->trackingAccount().first,
                          IdentifierType::ACCOUNT);
         if (match[3] != "")
         {
            entry->setAmount(parseCurrency(match[3]));
         }
         m_activeTransaction->appendEntry(entry);
         break;
      }
      case IdentifierType::OWNER:
      {
         auto entry = make_shared<LedgerTransactionV2OwnerEntry>(*m_activeItem);
         entry->setOwner(match[1]);
         entry->setTrackingAccount(match[2]);
         verifyIdentifier(entry->trackingAccount().first,
                          IdentifierType::ACCOUNT);
         if (match[3] != "")
         {
            entry->setAmount(parseCurrency(match[3]));
         }
         m_activeTransaction->appendEntry(entry);
         break;
      }
   }
}

bool FileReader::hasLines(std::ifstream& file)
{
   return !m_lines.empty() || !file.eof();
}

string FileReader::readLine(std::ifstream& file)
{
   if (!m_lines.empty())
   {
      string line(m_lines.top());
      m_lines.pop();
      ++m_lineNum;
      return line;
   }
   else if (file)
   {
      ++m_lineNum;
      string line;
      getline(file, line);
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
   bool ok;
   Currency c(Currency::fromString(currency, &ok));
   if (!ok)
   {
      stringstream ss;
      ss << "Unable to parse currency '" << currency << "'";
      die(m_fileName, m_lineNum, ss.str());
   }
   return c;
}

Date FileReader::parseDate(string const& date)
{
   try
   {
      return Date::fromString(date, m_dateFormat);
   }
   catch (...)
   {
      stringstream ss;
      ss << "Unable to parse date '" << date << "', expected something like '"
         << m_dateFormat << "'";
      die(m_fileName, m_lineNum, ss.str());
   }
}

Interval FileReader::parseInterval(string const& interval)
{
   try
   {
      return Interval::fromString(interval);
   }
   catch (...)
   {
      stringstream ss;
      ss << "Unable to parse interval '" << interval << "'";
      die(m_fileName, m_lineNum, ss.str());
   }
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
