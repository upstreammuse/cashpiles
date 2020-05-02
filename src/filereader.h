#pragma once

#include <fstream>
#include <regex>
#include <stack>

#include "ledgeraccount.h"

class Currency;
class Date;
class Interval;
class Ledger;

class FileReader
{
public:
   static Currency parseCurrency(std::string const& currency,
                                 std::string const& fileName, size_t lineNum);
   static Date parseDate(std::string const& date, std::string const& dateFormat,
                         std::string const& fileName, size_t lineNum);

public:
   FileReader(std::string const& fileName, Ledger& ledger);
   void readAll();
   void setDateFormat(std::string const& dateFormat);

private:
   void processAccount(std::smatch const& match);
   void processBlank();
   void processBudget(std::smatch& match);
   void processComment(std::smatch const& match);
   void processCompactTransaction(std::smatch const& match);
   void processCompactTransactionOff(std::smatch const& match);
   void processLine(std::string const& line);
   void processTransaction(std::smatch& match);

   bool hasLines();
   std::string readLine();
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
   std::string m_dateFormat = "yyyy/MM/dd";
   std::ifstream m_file;
   std::string m_fileName;
   std::map<std::string, IdentifierType> m_identifiers;
   Ledger& m_ledger;
   size_t m_lineNum = 0;
   std::stack<std::string> m_lines;
};
