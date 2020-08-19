#pragma once

#include <fstream>
#include <map>
#include <regex>
#include <stack>
#include "ledgeraccount.h"
#include "filereaderregex.h"

class Currency;
class Date;
class Interval;
class Ledger;
class LedgerBudget;
class LedgerTransactionV2;

class FileReader
{
public:
   static Currency parseCurrency(std::string const& currency,
                                 std::string const& fileName, size_t lineNum);
   static Date parseDate(std::string const& date, std::string const& dateFormat,
                         std::string const& fileName, size_t lineNum);

public:
   FileReader();
   void readAll(Ledger& ledger, std::string const& fileName);
   void setDateFormat(std::string const& dateFormat);

private:
   void processAccount(Ledger&, std::smatch const& match);
   void processAccountBalance(Ledger&, std::smatch const& match);
   void processBlank(Ledger&);
   void processBudget(Ledger&, std::smatch& match);
   void processComment(Ledger&, std::smatch const& match);
   void processCompactTransactionOff(Ledger&, std::smatch const& match);
   void processLine(Ledger&, std::string const& line);
   void processTransactionV2(Ledger&, std::smatch& match);

   bool hasLines(std::ifstream&);
   std::string readLine(std::ifstream&);
   void unReadLine(std::string const& line);

private:
   enum class IdentifierType
   {
      ACCOUNT,
      CATEGORY,
      OWNER
   };

private:
   IdentifierType identifierType(std::string const& identifier);
   Currency parseCurrency(std::string const& currency);
   Date parseDate(std::string const& date);
   Interval parseInterval(std::string const& interval);
   LedgerAccount::Mode parseMode(std::string const& mode);
   void verifyIdentifier(std::string const& identifier, IdentifierType type);
   void verifySetIdentifier(std::string const& identifier, IdentifierType type);

private:
   std::shared_ptr<LedgerBudget> m_activeBudget;
   std::shared_ptr<LedgerTransactionV2> m_activeTransaction;
   std::string m_dateFormat = "yyyy/MM/dd";
   std::string m_fileName;
   std::map<std::string, IdentifierType> m_identifiers;
   size_t m_lineNum = 0;
   std::stack<std::string> m_lines;
FileReaderRegEx m_regEx;
};
