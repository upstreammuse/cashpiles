#include "modelreader.h"

#include <cassert>
#include <memory>
#include <regex>
#include "m_currency.h"
#include "model.h"
#include "modelreaderformat.h"
#include "modelregex.h"
#include "rubbish.h"
#include "transaction.h"
#include "transactionflag.h"
#include "transactionflaginvalid.h"

using namespace model;

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

Currency ModelReader::parseCurrency(string s)
{
   // remove currency symbol
   auto symbol = m_format.currencySymbol();
   auto pos = s.find(symbol);
   if (pos != string::npos)
   {
      s.erase(pos, symbol.size());
   }

   // remove separator symbols
   auto sep = m_format.groupSeparator();
   for (pos = s.find(sep); pos != string::npos; pos = s.find(sep, pos))
   {
      s.erase(pos, sep.size());
   }

   // make sure we only have digits, -, and .
   auto allowed = string { "0123456789" };
   allowed.append(m_format.decimalPoint());
   allowed.append(m_format.negativeSign());
   if (s.find_first_not_of(allowed) != string::npos)
   {
      throw Rubbish("currency has invalid characters");
   }

   // this is a stupid hack, but when we split the number at the decimal, we
   // end up reading "-0" when the number is less than a whole currency unit,
   // and that gets translated into a positive number
   bool negate = false;
   // TODO c++ version of atof?
   if (atof(s.c_str()) < 0)
   {
      negate = true;
   }

   // get the numbers before and after the decimal
   auto decimal = m_format.decimalPoint();
   string before;
   string after;
   pos = s.find(decimal);
   if (pos != string::npos)
   {
      before = string(s, 0, pos);
      after = string(s, pos + decimal.size());
   }
   else
   {
      before = s;
      after = "";
   }

   // make sure we either have no decimal, or the correct decimal digits
   if (after.size() != 0 && after.size() != m_format.currencyDecimalDigits())
   {
      throw Rubbish("currency has invalid number of decimal digits");
   }

   // turn the values into a currency class
   Currency retval;
   // make sure we are working with a positive number for now
   // TODO c++ variant of strtoll?
   retval.value = strtoll(before.c_str(), nullptr, 10);
   if (retval.value < 0)
   {
      retval.value = -retval.value;
   }
   for (size_t i = 0; i < after.size(); ++i)
   {
      retval.value *= 10;
   }
   retval.value += strtoll(after.c_str(), nullptr, 10);
   if (negate)
   {
      retval.value = -retval.value;
   }
   retval.decimalPlaces = after.size();
   return retval;
}

Date ModelReader::parseDate(string const& date)
{
   auto format = m_format.dateFormat();
   auto month = format.find("M");
   auto monthLeading = format.find("MM");
   auto day = format.find("d");
   auto dayLeading = format.find("dd");
   auto year = format.find("yyyy");

   if (monthLeading != string::npos)
   {
      month = monthLeading;
   }
   if (dayLeading != string::npos)
   {
      day = dayLeading;
   }

   string monthS;
   string dayS;
   string yearS;
   for (size_t i = 0, j = 0; i < date.size() && j < format.size(); /*inside*/)
   {
      if (j == month)
      {
         monthS.append(1, date[i]);
         ++i;
         ++j;
         if (monthLeading == string::npos)
         {
            if (i < date.size() && isdigit(date[i]))
            {
               monthS.append(1, date[i]);
               ++i;
            }
         }
         else
         {
            monthS.append(1, date[i]);
            ++i;
            ++j;
         }
      }
      else if (j == day)
      {
         dayS.append(1, date[i]);
         ++i;
         ++j;
         if (dayLeading == string::npos)
         {
            if (i < date.size() && isdigit(date[i]))
            {
               dayS.append(1, date[i]);
               ++i;
            }
         }
         else
         {
            dayS.append(1, date[i]);
            ++i;
            ++j;
         }
      }
      else if (j == year)
      {
         yearS.append(date.c_str() + i, 4);
         i += 4;
         j += 4;
      }
      else if (date[i] == format[j])
      {
         ++i;
         ++j;
      }
      else
      {
         throw Rubbish("Date doesn't match format string");
      }
   }

   auto digits = "01234567889";
   if (monthS.find_first_not_of(digits) != string::npos)
   {
      throw Rubbish("date doesn't have month");
   }
   if (dayS.find_first_not_of(digits) != string::npos)
   {
      throw Rubbish("date doesn't have day");
   }
   if (yearS.find_first_not_of(digits) != string::npos)
   {
      throw Rubbish("date doesn't have year");
   }

   // days in 1 year
   auto days1year = 365;
   // days in 4 year cycle
   auto days4years = days1year * 4 + 1;
   // days in 100 year cycle
   auto days100years = days4years * 25 - 1;
   // days in 400 year cycle
   auto days400years = days100years * 4 + 1;

   int daysInMonth[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

   auto yearNum = strtol(yearS.c_str(), nullptr, 10);
   if (yearNum % 400 == 0 || (yearNum % 4 == 0 && yearNum % 100 != 0))
   {
      daysInMonth[2] = 29;
   }
   auto monthNum = strtol(monthS.c_str(), nullptr, 10);
   auto dayNum = strtol(dayS.c_str(), nullptr, 10);

   long long int dateNum = yearNum / 400 * days400years;
   yearNum %= 400;
   dateNum += yearNum / 100 * days100years;
   yearNum %= 100;
   dateNum += yearNum / 4 * days4years;
   yearNum %= 4;
   dateNum += yearNum * days1year;
   for (int i = 1; i <= monthNum; ++i)
   {
      dateNum += daysInMonth[i];
   }
   dateNum += dayNum;
   return Date(dateNum);
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

void ModelReader::processAccount(Model& model, smatch const& match,
                                 string const& comment)
{
   verifySetIdentifier(match[3], IdentifierType::ACCOUNT);

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
   auto date = parseDate(match[1]);
   auto account = match[2];
   auto amount = parseCurrency(match[3]);
   model.createAccountStatement(date, account, amount, comment);
}

void ModelReader::processBlank(Model& model, string const& comment)
{
   model.createBlank(comment);
}

#if 0
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
      else
      {
         unReadLine(line);
         break;
      }
   }

   m_ledger.appendItem(budget);
}
#endif

void ModelReader::processLine(Model& model, string& line)
{
   // TODO why doesn't this live with the other regex?
   // strings without ';' that end in a non-space character, possible space,
   // then an optional comment starting with ';' and ending with a non-space
   // character, possible space
   auto lineRx = regex {"^([^;]*[^\\s;])\\s*(?:;(.*\\S))?\\s*"};

   string comment;
   smatch match;
   if (regex_match(line, match, lineRx))
   {
      line = match[1];
      comment = match[2];
   }

   if (m_activeBudget)
   {
      // TODO read budget lines instead
      return;
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
      return;
   }

   if (regex_match(line, match, m_regex.accountRx))
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
      throw Rubbish("invalid line '" + line + "'");
   }
}

void ModelReader::processReferenceTransaction(
      Model& model, smatch const& match, string const& note)
{
   auto date = parseDate(match[1]);
   auto flag = parseFlag(match[2]);
   auto account = match[3];
   verifyIdentifier(account, IdentifierType::ACCOUNT);
   auto payee = match[4];
   auto amount = parseCurrency(match[5]);

   model.createReferenceTransaction(date, flag, account, payee, amount, note);
}

void ModelReader::processTransaction(
      Model& model, smatch const& match, string const& note)
{
   auto date = parseDate(match[1]);
   auto flag = parseFlag(match[2]);
   auto payee = match[3];

   m_activeTransaction = model.createTransaction(date, flag, payee, note);
}

void ModelReader::processTransactionLine(
      Model& model, smatch const& match, string const& note)
{
   assert(m_activeTransaction);
   switch (identifierType(match[1]))
   {
      case IdentifierType::ACCOUNT:
      {
         auto account = match[1];
         // TODO this might throw if it has nothing
         auto amount = parseCurrency(match[2]);
         model.createAccountLine(
                  m_activeTransaction->id, account, amount, note);
         break;
      }
      case IdentifierType::CATEGORY:
      {
         auto category = match[1];
         // TODO this might throw if it has nothing
         auto amount = parseCurrency(match[2]);
         model.createCategoryLine(
                  m_activeTransaction->id, category, amount, note);
         break;
      }
      case IdentifierType::OWNER:
      {
         auto owner = match[1];
         // TODO this might throw if it has nothing
         auto amount = parseCurrency(match[2]);
         model.createOwnerLine(m_activeTransaction->id, owner, amount, note);
         break;
      }
   }
}

void ModelReader::processTransactionTrackingLine(
      Model& model, smatch const& match, string const& note)
{
   switch (identifierType(match[1]))
   {
      case IdentifierType::ACCOUNT:
      {
         throw Rubbish("Cannot use tracking account X with account Y");
      }
      case IdentifierType::CATEGORY:
      {
         auto category = match[1];
         auto trackingAccount = match[2];
         // TODO need to figure out if this level of validation is contrary to what the model validation does, and which is correct if so (e.g. can an account and an owner have the same name?  The main model should decide how that works, not here...but there's a level of needing to know, so that we know whether to try something as a category or as an account, for example .... BUT the model could have queries to tell us what a string corresponds to, it's 'type' if you will)
         verifyIdentifier(trackingAccount, IdentifierType::ACCOUNT);
         // TODO this might throw if empty
         auto amount = parseCurrency(match[3]);
         model.createCategoryTrackingLine(
                  m_activeTransaction->id, category, trackingAccount, amount,
                  note);
         break;
      }
      case IdentifierType::OWNER:
      {
         auto owner = match[1];
         auto trackingAccount = match[2];
         // TODO need to figure out if this level of validation is contrary to what the model validation does, and which is correct if so (e.g. can an account and an owner have the same name?  The main model should decide how that works, not here...but there's a level of needing to know, so that we know whether to try something as a category or as an account, for example .... BUT the model could have queries to tell us what a string corresponds to, it's 'type' if you will)
         verifyIdentifier(trackingAccount, IdentifierType::ACCOUNT);
         // TODO this might throw if empty
         auto amount = parseCurrency(match[3]);
         // TODO clean up the terminology so there is one thing in CP to call this
         model.createOwnerTrackingLine(
                  m_activeTransaction->id, owner, trackingAccount, amount,
                  note);
         break;
      }
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

ModelReader::IdentifierType ModelReader::identifierType(
      string const& identifier)
{
   if (!m_identifiers.count(identifier))
   {
      throw Rubbish("Unknown identifier '" + identifier + "'");
   }
   return m_identifiers[identifier];
}

#if 0
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
#endif

void ModelReader::verifyIdentifier(
      string const& identifier, IdentifierType type)
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
      throw Rubbish(ss.str());
   }
}

void ModelReader::verifySetIdentifier(
      string const& identifier, IdentifierType type)
{
   if (!m_identifiers.count(identifier))
   {
      m_identifiers[identifier] = type;
   }
   verifyIdentifier(identifier, type);
}

#include "model.h"

void testMain()
{
   Model model;
   ModelReaderFormat format("M/dd/yyyy");
   ModelReader reader("Z:\\CashPiles\\CashPiles-Us.txt", format);
   reader.readModel(model);
}
