#pragma once

#include <fstream>
#include <memory>
#include <stack>
#include <string>
#include "../currency.h"
#include "../date.h"
#include "modelreaderformat.h"
#include "modelregex.h"

namespace model { struct BudgetPeriod; }
namespace model { struct Interval; }
namespace model { class Model; }
namespace model { class ModelReader; }
namespace model { struct Transaction; }
namespace model { enum class TransactionFlag; }

class model::ModelReader
{
public:
   ModelReader(std::string const& fileName, ModelReaderFormat const&);
   void readModel(Model&);

private:
   bool hasLines(std::ifstream&);
   TransactionFlag parseFlag(std::string const&);
   Interval parseInterval(std::string const&);
   void processAccount(Model&, std::smatch const&, std::string const&);
   void processAccountBalance(Model&, std::smatch const&, std::string const&);
   void processBlank(Model&, std::string const&);
   void processBudget(Model&, std::smatch const&, std::string const&);
   void processBudgetCancel(Model&, std::smatch const&, std::string const&);
   void processBudgetIncome(Model&, std::smatch const&, std::string const&);
   void processLine(Model&, std::string&);
   void processReferenceTransaction(
         Model&, std::smatch const&, std::string const&);
   void processTransaction(Model&, std::smatch const&, std::string const&);
   void processTransactionLine(Model&, std::smatch const&, std::string const&);
   void processTransactionTrackingLine(
         Model&, std::smatch const&, std::string const&);
   std::string readLine(std::ifstream&);
   void unReadLine(std::string const& line);

private:
   std::shared_ptr<BudgetPeriod const> m_activeBudget;
   std::shared_ptr<Transaction const> m_activeTransaction;
   std::string m_fileName;
   ModelReaderFormat m_format;
   size_t m_lineNum = 0;
   std::stack<std::string> m_lines;
   ModelRegex m_regex;
};
