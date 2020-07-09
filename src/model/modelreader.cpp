#include "modelreader.h"

#include <cassert>
#include <memory>
#include <regex>
#include "budgetcategoryentry.h"
#include "budgetgoalentry.h"
#include "budgetgoalsentry.h"
#include "budgetperiod.h"
#include "budgetuninitialized.h"
#include "../util/m_currency.h"
#include "m_daterange.h"
#include "m_interval.h"
#include "model.h"
#include "modelreaderformat.h"
#include "modelregex.h"
#include "rubbish.h"
#include "transaction.h"
#include "transactionflag.h"
#include "transactionflaginvalid.h"

using namespace model;
using namespace util;

using std::ifstream;
using std::make_shared;
using std::regex;
using std::smatch;
using std::string;
using std::stringstream;

ModelReader::ModelReader(string const& fileName,
                         ModelReaderFormat const& format) :
   m_fileName(fileName),
   m_format(format),
   m_regex(format)
{
}

void ModelReader::readModel(Model& model)
{
   m_lineNum = 0;
   while (m_lines.size())
   {
      m_lines.pop();
   }

   ifstream file(m_fileName);
   if (!file)
   {
      throw Rubbish("can't read file, or file is empty?");
   }
   while (hasLines(file))
   {
      auto line = readLine(file);
      processLine(model, line);
   }
}

bool ModelReader::hasLines(ifstream& file)
{
   return !m_lines.empty() || !file.eof();
}

TransactionFlag ModelReader::parseFlag(string const& flag)
{
   switch (flag[0])
   {
      case '*':
         return TransactionFlag::CLEARED;
      case '!':
         return TransactionFlag::DISPUTED;
      case '?':
         return TransactionFlag::PENDING;
      default:
         throw TransactionFlagInvalid(flag);
   }
}

Interval ModelReader::parseInterval(string const& interval)
{
   if (interval.substr(0, 1) != "+")
   {
      throw Rubbish("Cannot parse interval");
   }

   auto number = std::stoull(interval.substr(1, interval.size() - 2), nullptr,
                             10);

   auto periodStr = interval.substr(interval.size() - 1);
   Interval::Period period;
   if (periodStr == "d")
   {
      period = Interval::Period::DAYS;
   }
   else if (periodStr == "w")
   {
      period = Interval::Period::DAYS;
      number *= 7;
   }
   else if (periodStr == "m")
   {
      period = Interval::Period::MONTHS;
   }
   else if (periodStr == "y")
   {
      period = Interval::Period::YEARS;
   }
   else
   {
      throw Rubbish("Cannot parse interval");
   }

   return Interval(number, period);
}

void ModelReader::processAccount(Model& model, smatch const& match,
                                 string const& comment)
{
   if (match[2] == "on-budget")
   {
      model.createBudgetAccount(match[3], comment);
   }
   else if (match[2] == "off-budget")
   {
      model.createReferenceAccount(match[3], comment);
   }
   else if (match[2] == "closed")
   {
      model.closeAccount(match[3], comment);
   }
}

void ModelReader::processAccountBalance(Model& model, smatch const& match,
                                        string const& comment)
{
   auto date = Date::parseDate(match[1], m_format.dateFormat);
   auto account = match[2];
   auto amount = Currency::parseCurrency(match[3], m_format.currencyFormat);
   model.createAccountStatement(date, account, amount, comment);
}

void ModelReader::processBlank(Model& model, string const& comment)
{
   model.createBlank(comment);
}

void ModelReader::processBudget(
      Model& model, smatch const& match, string const& note)
{
   auto date = Date::parseDate(match[1], m_format.dateFormat);
   auto interval = parseInterval(match[2]);
   try
   {
      m_activeBudget = model.growBudgetPeriods(date);
      model.configureBudget(m_activeBudget->id, interval, note);
   }
   catch (BudgetUninitialized const&)
   {
      m_activeBudget = model.initializeBudget(date, interval, note);
   }
}

void ModelReader::processBudgetCancel(
      Model& model, smatch const& match, string const& note)
{
   auto goal = model.budgetGoal(match[1], match[2]);
   model.cancelGoal(goal->category->name, goal->name, note);
}

void ModelReader::processBudgetIncome(
      Model& model, smatch const& match, string const& note)
{
   auto budget = model.getCurrentBudget();
   model.createBudgetIncomeEntry(budget->id, match[1], match[2], note);
}

void ModelReader::processLine(Model& model, string& line)
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

   string comment;
   auto loc = line.find(';');
   if (loc != string::npos)
   {
      comment = line.substr(loc + 1);
      line = line.substr(0, loc);
   }
   while (isspace(line.back()))
   {
      line = line.substr(0, line.size() - 1);
   }
   while (isspace(comment.back()))
   {
      comment = comment.substr(0, comment.size() - 1);
   }

   smatch match;
   if (m_activeBudget)
   {
      if (regex_match(line, match, m_regex.budgetLineCancelRx))
      {
         processBudgetCancel(model, match, comment);
      }
      else if (regex_match(line, match, m_regex.budgetLineIncomeRx))
      {
         processBudgetIncome(model, match, comment);
      }
      else
      {
         unReadLine(line);
         m_activeBudget.reset();
      }


#if 0

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
#endif







   }
   else if (m_activeTransaction)
   {
      if (regex_match(line, match, m_regex.txnLineRx))
      {
         processTransactionLine(model, match, comment);
      }
      else if (regex_match(line, match, m_regex.txnTrackingLineRx))
      {
         processTransactionTrackingLine(model, match, comment);
      }
      else
      {
         unReadLine(line);
         model.finalizeTransaction(m_activeTransaction->id);
         m_activeTransaction.reset();
      }
   }
   else if (regex_match(line, match, m_regex.accountRx))
   {
      processAccount(model, match, comment);
   }
   else if (regex_match(line, match, m_regex.accountBalanceRx))
   {
      processAccountBalance(model, match, comment);
   }
   else if (regex_match(line, match, m_regex.budgetRx))
   {
      processBudget(model, match, comment);
   }
   else if (regex_match(line, match, m_regex.txnRx))
   {
      processTransaction(model, match, comment);
   }
   else if (regex_match(line, match, m_regex.txnRefRx))
   {
      processReferenceTransaction(model, match, comment);
   }
   else if (line == "")
   {
      processBlank(model, comment);
   }
   else
   {
      throw Rubbish("invalid line '" + line + "', comment '" + comment + "'");
   }
}

void ModelReader::processReferenceTransaction(
      Model& model, smatch const& match, string const& note)
{
   auto date = Date::parseDate(match[1], m_format.dateFormat);
   auto flag = parseFlag(match[2]);
   auto account = match[3];
   auto payee = match[4];
   auto amount = Currency::parseCurrency(match[5], m_format.currencyFormat);

   model.createReferenceTransaction(date, flag, account, payee, amount, note);
}

void ModelReader::processTransaction(
      Model& model, smatch const& match, string const& note)
{
   auto date = Date::parseDate(match[1], m_format.dateFormat);
   auto flag = parseFlag(match[2]);
   auto payee = match[3];

   m_activeTransaction = model.createTransaction(date, flag, payee, note);
}

void ModelReader::processTransactionLine(
      Model& model, smatch const& match, string const& note)
{
   assert(m_activeTransaction);
   auto identifier = match[1];
   // TODO this might throw if it has nothing
   auto amount = Currency::parseCurrency(match[2], m_format.currencyFormat);
   switch (model.getIdentifierType(match[1]))
   {
      case Model::IdentifierType::ACCOUNT:
         model.createAccountEntry(
                  m_activeTransaction->id, identifier, amount, note);
         break;
      case Model::IdentifierType::CATEGORY:
         model.createCategoryEntry(
                  m_activeTransaction->id, identifier, amount, note);
         break;
      case Model::IdentifierType::OWNER:
         model.createOwnerEntry(
                  m_activeTransaction->id, identifier, amount, note);
         break;
   }
}

void ModelReader::processTransactionTrackingLine(
      Model& model, smatch const& match, string const& note)
{
   auto identifier = match[1];
   auto trackingAccount = match[2];
   // TODO this might throw if empty
   auto amount = Currency::parseCurrency(match[3], m_format.currencyFormat);
   switch (model.getIdentifierType(match[1]))
   {
      case Model::IdentifierType::ACCOUNT:
         throw Rubbish("Cannot use tracking account X with account Y");
      case Model::IdentifierType::CATEGORY:
         model.createCategoryTrackingEntry(
                  m_activeTransaction->id, identifier, trackingAccount, amount,
                  note);
         break;
      case Model::IdentifierType::OWNER:
         // TODO clean up the terminology so there is one thing in CP to call
         // this, 'tracking', or 'reference', or somethign else?
         model.createOwnerTrackingEntry(
                  m_activeTransaction->id, identifier, trackingAccount, amount,
                  note);
         break;
   }
}

string ModelReader::readLine(ifstream& file)
{
   if (!m_lines.empty())
   {
      auto line = m_lines.top();
      m_lines.pop();
      ++m_lineNum;
      return line;
   }
   else if (file)
   {
      string line;
      getline(file, line);
      ++m_lineNum;
      return line;
   }
   else
   {
      return "";
   }
}

void ModelReader::unReadLine(string const& line)
{
   --m_lineNum;
   m_lines.push(line);
}

#include <iostream>
#include "model.h"

void testMain()
{
   try
   {
      Model model;
      ModelReaderFormat format("M/d/yyyy");
      ModelReader reader("Z:\\CashPiles\\CashPiles-Us.txt", format);
      reader.readModel(model);
   }
   catch (std::logic_error const& ex)
   {
      std::cerr << ex.what() << std::endl;
   }
}
