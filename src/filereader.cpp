#include "filereader.h"

#include <regex>
#include <sstream>
#include "cashpiles.h"
#include "date.h"
#include "ledger.h"
#include "ledgeraccount.h"
#include "ledgerblank.h"
#include "ledgerbudget.h"
#include "ledgerbudgetcloseentry.h"
#include "ledgerbudgetentry.h"
#include "ledgerbudgetgoalentry.h"
#include "ledgerbudgetincomeentry.h"
#include "ledgerbudgetreserveamountentry.h"
#include "ledgerbudgetreservepercententry.h"
#include "ledgerbudgetroutineentry.h"
#include "ledgerbudgetwithholdingentry.h"
#include "ledgercomment.h"
#include "ledgererror.h"
#include "ledgerreserve.h"
#include "ledgertransaction.h"

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
   std::regex const budgetLineCloseRx;
   std::regex const budgetLineGoalRx;
   std::regex const budgetLineIncomeRx;
   std::regex const budgetLineReserveAmountRx;
   std::regex const budgetLineReservePercentRx;
   std::regex const budgetLineRoutineRx;
   std::regex const budgetLineWithholdingRx;
   std::regex const commentRx;
   std::regex const reserveCompactRx;
   std::regex const reserveRx;
   std::regex const reserveLineRx;
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
      budgetLineCloseRx(
         START_RX + SEP_RX + "close" + SPACE_RX + IDENT_RX + END_RX),
      budgetLineGoalRx(
         START_RX + SEP_RX + "goal" + SPACE_RX + IDENT_RX +
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
      reserveCompactRx(
         START_RX + DATE_RX + SPACE_RX + "reserve" + SPACE_RX +
         IDENT_RX + SEP_RX + CURR_RX + END_RX),
      reserveRx(
         START_RX + DATE_RX + SPACE_RX + "reserve" + END_RX),
      reserveLineRx(
         START_RX + SEP_RX + IDENT_RX + SEP_RX +
         CURR_RX + optional(SPACE_RX + NOTE_RX) + END_RX),
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

auto FileReader::parseMode(std::smatch const& match, size_t index,
                           std::shared_ptr<LedgerError> error)
{
   bool ok;
   auto mode = LedgerAccount::modeFromString(match[index], &ok);
   if (!ok)
   {
      error->insertMessage("Unknown account command '" + match.str(index) + "'",
      {m_fileName, m_lineNum, size_t(match.position(index))});
   }
   return mode;
}

// TODO not sure how I feel about putting this up here
auto FileReader::parseTxnStatus(
      std::smatch const& match,
      size_t index,
      std::shared_ptr<LedgerError> error)
{
   bool ok;
   auto status = LedgerTransaction::statusFromString(match[index], &ok);
   if (!ok)
   {
      error->insertMessage("Unexpected transaction status '" +
                           match.str(index) + "'",
      {m_fileName, m_lineNum, size_t(match.position(index))});
   }
   return status;
}

FileReader::FileReader(std::string const& fileName, Ledger& ledger) :
   m_fileName(fileName),
   m_ledger(ledger)
{
   regEx = new FileReaderRegEx;
}

void FileReader::readAll()
{
   m_ledger.clear();
   m_file.open(m_fileName);
   if (!m_file)
   {
      std::stringstream ss;
      ss << "Unable to open input file '" << m_fileName << "'";
      auto error = std::make_shared<LedgerError>();
      error->insertMessage(ss.str(), {m_fileName, 0, 0});
      m_ledger.appendItem(error);
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
   auto error = std::make_shared<LedgerError>();
   error->insertContent(match[0], {m_fileName, m_lineNum, 0});

   auto account = std::make_shared<LedgerAccount>(m_fileName, m_lineNum);
   account->setDate(parseDate(match, 1, error));
   account->setMode(parseMode(match, 2, error));
   account->setName(Identifier(match.str(3), Identifier::Type::ACCOUNT));

   if (error->hasMessages())
   {
      m_ledger.appendItem(error);
   }
   else
   {
      m_ledger.appendItem(account);
   }
}

void FileReader::processBlank()
{
   std::shared_ptr<LedgerBlank> blank(new LedgerBlank(m_fileName, m_lineNum));
   m_ledger.appendItem(blank);
}

void FileReader::processBudget(std::smatch& match)
{
   auto error = std::make_shared<LedgerError>();
   error->insertContent(match[0], {m_fileName, m_lineNum, 0});
   bool ok;

   std::shared_ptr<LedgerBudget> budget(
            new LedgerBudget(m_fileName, m_lineNum));
   budget->setDate(parseDate(match, 1, error));
   budget->setInterval(parseInterval(match, 2, error));

   while (true)
   {
      std::string line(readLine());
      if (std::regex_match(line, match, regEx->budgetLineCloseRx))
      {
         std::shared_ptr<LedgerBudgetCloseEntry> entry(
                  new LedgerBudgetCloseEntry(m_fileName, m_lineNum));
         entry->setCategory(Identifier(match.str(1),
                                       Identifier::Type::CATEGORY));
         budget->appendEntry(entry);
         error->insertContent(line, {m_fileName, m_lineNum, 0});
      }
      else if (std::regex_match(line, match, regEx->budgetLineGoalRx))
      {
         std::shared_ptr<LedgerBudgetGoalEntry> entry(
                  new LedgerBudgetGoalEntry(m_fileName, m_lineNum));
         entry->setCategory(Identifier(match.str(1),
                                       Identifier::Type::CATEGORY));
         entry->setOwner(Identifier(match.str(2),
                                    Identifier::Type::OWNER));
         budget->appendEntry(entry);
         error->insertContent(line, {m_fileName, m_lineNum, 0});
      }
      else if (std::regex_match(line, match, regEx->budgetLineIncomeRx))
      {
         std::shared_ptr<LedgerBudgetIncomeEntry> entry(
                  new LedgerBudgetIncomeEntry(m_fileName, m_lineNum));
         entry->setCategory(Identifier(match.str(1),
                                       Identifier::Type::CATEGORY));
         entry->setOwner(Identifier(match.str(2),
                                    Identifier::Type::OWNER));
         budget->appendEntry(entry);
         error->insertContent(line, {m_fileName, m_lineNum, 0});
      }
      else if (std::regex_match(line, match, regEx->budgetLineReserveAmountRx))
      {
         std::shared_ptr<LedgerBudgetReserveAmountEntry> entry(
                  new LedgerBudgetReserveAmountEntry(m_fileName, m_lineNum));
         entry->setCategory(Identifier(match.str(1),
                                       Identifier::Type::CATEGORY));
         entry->setOwner(Identifier(match.str(4),
                                    Identifier::Type::OWNER));
         entry->setAmount(Currency::fromString(match.str(2), &ok));
         if (!ok)
         {
            error->insertMessage("Error in currency '" + match.str(2) + "'",
            {m_fileName, m_lineNum, size_t(match.position(2))});
         }
         entry->setInterval(parseInterval(match, 3, error));
         budget->appendEntry(entry);
         error->insertContent(line, {m_fileName, m_lineNum, 0});
      }
      else if (std::regex_match(line, match, regEx->budgetLineReservePercentRx))
      {
         std::shared_ptr<LedgerBudgetReservePercentEntry> entry(
                  new LedgerBudgetReservePercentEntry(m_fileName, m_lineNum));
         entry->setCategory(Identifier(match.str(1),
                                       Identifier::Type::CATEGORY));
         entry->setOwner(Identifier(match.str(3),
                                    Identifier::Type::OWNER));
         entry->setPercentage(std::stoul(match.str(2), nullptr, 10));
         budget->appendEntry(entry);
         error->insertContent(line, {m_fileName, m_lineNum, 0});
      }
      else if (std::regex_match(line, match, regEx->budgetLineRoutineRx))
      {
         std::shared_ptr<LedgerBudgetRoutineEntry> entry(
                  new LedgerBudgetRoutineEntry(m_fileName, m_lineNum));
         entry->setCategory(Identifier(match.str(1),
                                       Identifier::Type::CATEGORY));
         entry->setOwner(Identifier(match.str(2),
                                    Identifier::Type::OWNER));
         budget->appendEntry(entry);
         error->insertContent(line, {m_fileName, m_lineNum, 0});
      }
      else if (std::regex_match(line, match, regEx->budgetLineWithholdingRx))
      {
         std::shared_ptr<LedgerBudgetWithholdingEntry> entry(
                  new LedgerBudgetWithholdingEntry(m_fileName, m_lineNum));
         entry->setCategory(Identifier(match.str(1),
                                       Identifier::Type::CATEGORY));
         entry->setOwner(Identifier(match.str(2),
                                    Identifier::Type::OWNER));
         budget->appendEntry(entry);
         error->insertContent(line, {m_fileName, m_lineNum, 0});
      }
      else
      {
         unReadLine(line);
         break;
      }
   }

   if (error->hasMessages())
   {
      m_ledger.appendItem(error);
   }
   else
   {
      m_ledger.appendItem(budget);
   }
}

void FileReader::processComment(std::smatch const& match)
{
   std::shared_ptr<LedgerComment> comment(
            new LedgerComment(m_fileName, m_lineNum));
   comment->setNote(match.str(1));
   m_ledger.appendItem(comment);
}

void FileReader::processCompactReserve(std::smatch const& match)
{
   auto error = std::make_shared<LedgerError>();
   error->insertContent(match[0], {m_fileName, m_lineNum, 0});
   bool ok;

   std::shared_ptr<LedgerReserve> reserve(
            new LedgerReserve(m_fileName, m_lineNum));
   reserve->setDate(parseDate(match, 1, error));

   std::shared_ptr<LedgerReserveEntry> entry(
            new LedgerReserveEntry(m_fileName, m_lineNum));
   entry->setCategory(Identifier(match.str(2),
                                 Identifier::Type::CATEGORY));
   entry->setAmount(Currency::fromString(match.str(3), &ok));
   if (!ok)
   {
      error->insertMessage("Error in currency '" + match.str(3) + "'",
      {m_fileName, m_lineNum, size_t(match.position(3))});
   }
   reserve->appendEntry(entry);

   if (error->hasMessages())
   {
      m_ledger.appendItem(error);
   }
   else
   {
      m_ledger.appendItem(reserve);
   }
}

void FileReader::processCompactTransaction(std::smatch const& match)
{
   auto error = std::make_shared<LedgerError>();
   error->insertContent(match[0], {m_fileName, m_lineNum, 0});
   bool ok;

   std::shared_ptr<LedgerTransaction> transaction(
            new LedgerTransaction(m_fileName, m_lineNum));
   transaction->setAccount(
            Identifier(match.str(3), Identifier::Type::ACCOUNT));

   if (match.str(7) != "")
   {
      transaction->setBalance(Currency::fromString(match.str(7), &ok));
      if (!ok)
      {
         error->insertMessage("Error in currency '" + match.str(7) + "'",
         {m_fileName, m_lineNum, size_t(match.position(7))});
      }
   }

   transaction->setStatus(transaction->statusFromString(match.str(2), &ok));
   if (!ok)
   {
      error->insertMessage("Error in transaction status '" + match.str(2) + "'",
      {m_fileName, m_lineNum, size_t(match.position(2))});
   }

   transaction->setDate(parseDate(match, 1, error));
   if (match.str(8) != "")
   {
      transaction->setNote(match.str(8));
   }

   LedgerTransactionEntry entry;
   entry.setAmount(Currency::fromString(match.str(6), &ok));
   if (!ok)
   {
      error->insertMessage("Error in currency '" + match.str(6) + "'",
      {m_fileName, m_lineNum, size_t(match.position(6))});
   }
   if (match.str(5)[0] == '@')
   {
      entry.setCategory(Identifier(match.str(5).substr(1),
                                   Identifier::Type::OWNER));
   }
   else
   {
      entry.setCategory(Identifier(match.str(5),
                                   Identifier::Type::CATEGORY));
   }
   if (match.str(4)[0] == '@')
   {
      entry.setPayee(Identifier(match.str(4).substr(1),
                                Identifier::Type::ACCOUNT));
   }
   else
   {
      entry.setPayee(Identifier(match.str(4),
                                Identifier::Type::GENERIC));
   }
   transaction->appendEntry(entry);

   if (error->hasMessages())
   {
      m_ledger.appendItem(error);
   }
   else
   {
      m_ledger.appendItem(transaction);
   }
}

void FileReader::processCompactTransactionOff(std::smatch const& match)
{
   auto error = std::make_shared<LedgerError>();
   error->insertContent(match[0], {m_fileName, m_lineNum, 0});
   bool ok;

   std::shared_ptr<LedgerTransaction> transaction(
            new LedgerTransaction(m_fileName, m_lineNum));
   transaction->setAccount(
            Identifier(match.str(3), Identifier::Type::ACCOUNT));
   if (match.str(6) != "")
   {
      transaction->setBalance(Currency::fromString(match.str(6), &ok));
      if (!ok)
      {
         error->insertMessage("Error in currency '" + match.str(6) + "'",
         {m_fileName, m_lineNum, size_t(match.position(6))});
      }
   }
   transaction->setStatus(
            transaction->statusFromString(match.str(2), &ok));
   if (!ok)
   {
      error->insertMessage("Error in transaction status '" + match.str(2) + "'",
      {m_fileName, m_lineNum, size_t(match.position(2))});
   }
   transaction->setDate(parseDate(match, 1, error));
   if (match.str(7) != "")
   {
      transaction->setNote(match.str(7));
   }

   LedgerTransactionEntry entry;
   entry.setAmount(Currency::fromString(match.str(5), &ok));
   if (!ok)
   {
      error->insertMessage("Error in currency '" + match.str(5) + "'",
      {m_fileName, m_lineNum, size_t(match.position(5))});
   }
   if (match.str(4)[0] == '@')
   {
      entry.setPayee(Identifier(match.str(4).substr(1),
                                Identifier::Type::ACCOUNT));
   }
   else
   {
      entry.setPayee(Identifier(match.str(4),
                                Identifier::Type::GENERIC));
   }
   transaction->appendEntry(entry);

   if (error->hasMessages())
   {
      m_ledger.appendItem(error);
   }
   else
   {
      m_ledger.appendItem(transaction);
   }
}

// TODO - imagine that the line is passed into the parsing code so that when an
// error occurrs the original text can be placed into the error item rather than
// the already-parsed content that might be collapsed somehow, and also the
// parsing code could return 'segments' of the line, which would be
// substr()-compatible offsets into the line, and those could be used for error
// reconstruction as well as for pulling the values needed for the data classes.
// looking for a natural way to embed the errors back into the ledger without
// losing any data, in the case you are running with dangerous overwrite mode,
// so that dates, currency, etc. are not lost if they are mis-processed.
// Thinking that if all is processed correctly, then the item is returned, but
// if something/anything goes wrong, then you return the original line along
// with the error, rather than attempting to wrap up a failed item which
// probably has lost data in its construction.  So makeError might not be
// needed (whatever), and the LedgerError would be a text-based output item that
// matches literal input to the errors that were generated.  And this could also
// be used for console output so that running in input-only mode can give the
// user context with their 'file x line y column z' error messages as well

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
   else if (std::regex_match(line, match, regEx->reserveCompactRx))
   {
      processCompactReserve(match);
   }
   else if (std::regex_match(line, match, regEx->txnCompactRx))
   {
      processCompactTransaction(match);
   }
   else if (std::regex_match(line, match, regEx->txnCompactOffRx))
   {
      processCompactTransactionOff(match);
   }
   else if (std::regex_match(line, match, regEx->reserveRx))
   {
      processReserve(match);
   }
   else if (std::regex_match(line, match, regEx->txnRx))
   {
      processTransaction(match);
   }
   else if (line == "")
   {
      processBlank();
   }
   else if (line == "ERROR")
   {
      auto error = std::make_shared<LedgerError>();
      while (true)
      {
         std::string line = readLine();
         if (line != "ERROR END")
         {
            error->insertContent(line, {m_fileName, m_lineNum, 0});
         }
         else
         {
            break;
         }
      }
      m_ledger.appendItem(error);
   }
   else
   {
      auto error = std::make_shared<LedgerError>();
      error->insertContent(line, {m_fileName, m_lineNum, 0});
      error->insertMessage("Invalid contents '" + line + "'",
      {m_fileName, m_lineNum, 0});
      m_ledger.appendItem(error);
   }
}

void FileReader::processReserve(std::smatch& match)
{
   auto error = std::make_shared<LedgerError>();
   error->insertContent(match[0], {m_fileName, m_lineNum, 0});
   bool ok;

   std::shared_ptr<LedgerReserve> reserve(
            new LedgerReserve(m_fileName, m_lineNum));
   reserve->setDate(parseDate(match, 1, error));

   while (true)
   {
      std::string line(readLine());
      if (std::regex_match(line, match, regEx->reserveLineRx))
      {
         std::shared_ptr<LedgerReserveEntry> entry(
                  new LedgerReserveEntry(m_fileName, m_lineNum));
         if (match.str(1)[0] == '@')
         {
            entry->setCategory(Identifier(match.str(1).substr(1),
                                          Identifier::Type::OWNER));
         }
         else
         {
            entry->setCategory(Identifier(match.str(1),
                                          Identifier::Type::CATEGORY));
         }

         entry->setAmount(Currency::fromString(match.str(2), &ok));
         if (!ok)
         {
            error->insertMessage("Error in currency '" + match.str(2) + "'",
            {m_fileName, m_lineNum, size_t(match.position(2))});
         }

         error->insertContent(line, {m_fileName, m_lineNum, 0});
         reserve->appendEntry(entry);
      }
      else
      {
         unReadLine(line);
         break;
      }
   }

   if (error->hasMessages())
   {
      m_ledger.appendItem(error);
   }
   else
   {
      m_ledger.appendItem(reserve);
   }
}

void FileReader::processTransaction(std::smatch& match)
{
   auto error = std::make_shared<LedgerError>();
   error->insertContent(match[0], {m_fileName, m_lineNum, 0});
   bool ok;

   std::shared_ptr<LedgerTransaction> xact(
            new LedgerTransaction(m_fileName, m_lineNum));
   xact->setAccount(
            Identifier(match.str(3), Identifier::Type::ACCOUNT));
   if (match.str(5) != "")
   {
      xact->setBalance(Currency::fromString(match.str(5), &ok));
      if (!ok)
      {
         error->insertMessage("Error in currency '" + match.str(5) + "'",
         {m_fileName, m_lineNum, size_t(match.position(5))});
      }
   }
   xact->setStatus(parseTxnStatus(match, 2, error));
   xact->setDate(parseDate(match, 1, error));
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
         entry.setAmount(Currency::fromString(match.str(3), &ok));
         if (!ok)
         {
            error->insertMessage("Error in currency '" + match.str(3) + "'",
            {m_fileName, m_lineNum, size_t(match.position(3))});
         }
         if (match.str(2)[0] == '@')
         {
            entry.setCategory(Identifier(match.str(2).substr(1),
                                         Identifier::Type::OWNER));
         }
         else
         {
            entry.setCategory(Identifier(match.str(2),
                                         Identifier::Type::CATEGORY));
         }
         if (match.str(4) != "")
         {
            entry.setNote(match.str(4));
         }
         if (match.str(1)[0] == '@')
         {
            entry.setPayee(Identifier(match.str(1).substr(1),
                                      Identifier::Type::ACCOUNT));
         }
         else
         {
            entry.setPayee(Identifier(match.str(1),
                                      Identifier::Type::GENERIC));
         }
         xact->appendEntry(entry);
         error->insertContent(line, {m_fileName, m_lineNum, 0});
      }
      else if (std::regex_match(line, match, regEx->txnLineOffRx))
      {
         LedgerTransactionEntry entry;
         entry.setAmount(Currency::fromString(match.str(2), &ok));
         if (!ok)
         {
            error->insertMessage("Error in currency '" + match.str(2) + "'",
            {m_fileName, m_lineNum, size_t(match.position(2))});
         }
         if (match.str(3) != "")
         {
            entry.setNote(match.str(3));
         }
         if (match.str(1)[0] == '@')
         {
            entry.setPayee(Identifier(match.str(1).substr(1),
                                      Identifier::Type::ACCOUNT));
         }
         else
         {
            entry.setPayee(Identifier(match.str(1),
                                      Identifier::Type::GENERIC));
         }
         xact->appendEntry(entry);
         error->insertContent(line, {m_fileName, m_lineNum, 0});
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

Date FileReader::parseDate(std::smatch const& match, size_t index,
                           std::shared_ptr<LedgerError> error)
{
   Date d(Date::fromString(match[index], m_dateFormat));
   if (!d.isValid())
   {
      error->insertMessage("Unable to process date '" + match.str(index) +
                           "', expected something like '" + m_dateFormat + "'",
      {m_fileName, m_lineNum, size_t(match.position(index))});
   }
   return d;
}

Interval FileReader::parseInterval(std::smatch const& match, size_t index,
                                   std::shared_ptr<LedgerError> error)
{
   bool ok;
   Interval i(Interval::fromString(match[index], &ok));
   if (!ok)
   {
      error->insertMessage("Unable to process interval '" + match.str(index) +
                           "'",
      {m_fileName, m_lineNum, size_t(match.position(index))});
   }
   return i;
}
