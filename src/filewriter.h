#pragma once

#include <fstream>
#include <string>
#include "itemprocessor.h"

class FileWriter : public ItemProcessor
{
public:
   explicit FileWriter(std::string const& fileName);
   void finish();
   void processItem(LedgerAccount const& account);
   void processItem(LedgerBlank const& blank);
   void processItem(LedgerBudget const& budget);
   void processItem(LedgerBudgetCancelEntry const& entry);
   void processItem(LedgerBudgetCloseEntry const& entry);
   void processItem(LedgerBudgetGoalEntry const& entry);
   void processItem(LedgerBudgetGoalsEntry const& entry);
   void processItem(LedgerBudgetIncomeEntry const& entry);
   void processItem(LedgerBudgetReserveAmountEntry const& entry);
   void processItem(LedgerBudgetReservePercentEntry const& entry);
   void processItem(LedgerBudgetRoutineEntry const& entry);
   void processItem(LedgerBudgetWithholdingEntry const& entry);
   void processItem(LedgerComment const& comment);
   void processItem(LedgerReserve const& reserve);
   void processItem(LedgerReserveEntry const& reserve);
   void processItem(LedgerTransaction const& transaction);
   void setDateFormat(std::string const& dateFormat);
   void start();

private:
   std::string m_dateFormat = "yyyy/MM/dd";
   std::ofstream m_file;
   std::string m_fileName;
   // TODO there needs to be a cleaner way than this
   bool m_singleReserve = false;
};
