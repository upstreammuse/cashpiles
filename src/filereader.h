#pragma once

#include <fstream>
#include <regex>
#include <stack>

#include "ledgeraccount.h"

class Currency;
class Date;
class Interval;
class Ledger;
class LedgerError;

class FileReader
{
public:
   FileReader(std::string const& fileName, Ledger& ledger);
   void readAll();
   void setDateFormat(std::string const& dateFormat);

private:
   void processAccount(std::smatch const& match);
   void processBlank();
   void processBudget(std::smatch& match);
   void processComment(std::smatch const& match);
   void processCompactReserve(std::smatch const& match);
   void processCompactTransaction(std::smatch const& match);
   void processCompactTransactionOff(std::smatch const& match);
   void processLine(std::string const& line);
   void processReserve(std::smatch& match);
   void processTransaction(std::smatch& match);

   bool hasLines();
   std::string readLine();
   void unReadLine(std::string const& line);

private:
   Date parseDate(std::smatch const& match, size_t index,
                  std::shared_ptr<LedgerError> error);
   Interval parseInterval(std::smatch const& match, size_t index,
                          std::shared_ptr<LedgerError> error);
   auto parseMode(std::smatch const& match, size_t index,
                  std::shared_ptr<LedgerError> error);
   auto parseTxnStatus(std::smatch const& match, size_t index,
                       std::shared_ptr<LedgerError> error);

private:
   std::string m_dateFormat = "yyyy/MM/dd";
   std::ifstream m_file;
   std::string m_fileName;
   Ledger& m_ledger;
   size_t m_lineNum = 0;
   std::stack<std::string> m_lines;
};
