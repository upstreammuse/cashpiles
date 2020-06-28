#pragma once

#include <fstream>
#include <memory>
#include <stack>
#include <string>
#include "m_date.h"
#include "modelreaderformat.h"
#include "modelregex.h"

namespace model { struct Currency; }
namespace model { class Model; }
namespace model { class ModelReader; }
namespace model { enum class TransactionFlag; }

class model::ModelReader
{
public:
   ModelReader(std::string const& fileName, ModelReaderFormat const&);
   void readModel(Model&);

private:
   bool hasLines(std::ifstream&);
   Currency parseCurrency(std::string);
   model::Date parseDate(std::string const&);
   TransactionFlag parseFlag(std::string const&);
   void processAccount(Model&, std::smatch const&, std::string const&);
   void processAccountBalance(Model&, std::smatch const&, std::string const&);
   void processBlank(Model&, std::string const&);
   void processBudget(Model&, std::smatch&, std::string const&);
   void processLine(Model&, std::string&);
   void processReferenceTransaction(
         Model&, std::smatch const&, std::string const&);
   void processTransaction(Model&, std::smatch&, std::string const&);
   std::string readLine(std::ifstream&);
//   void unReadLine(std::string const& line);

private:
   enum class IdentifierType
   {
      ACCOUNT,
      CATEGORY,
      OWNER
   };

private:
   IdentifierType identifierType(std::string const&);
//   Interval parseInterval(std::string const& interval);
   void verifyIdentifier(std::string const&, IdentifierType);
   void verifySetIdentifier(std::string const&, IdentifierType);

private:
   std::ifstream m_file;
   std::string m_fileName;
   ModelReaderFormat m_format;
   std::map<std::string, IdentifierType> m_identifiers;
   size_t m_lineNum = 0;
   std::stack<std::string> m_lines;
   ModelRegex m_regex;
};
