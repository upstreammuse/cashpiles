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
   void processItem(LedgerAccountBalance const& balance);
   void processItem(LedgerBlank const& blank);
   bool processItem(LedgerBudget const& budget);
   void processItem(LedgerBudgetCancelEntry const& entry);
   void processItem(LedgerBudgetCloseEntry const& entry);
   void processItem(LedgerBudgetGoalEntry const& entry);
   void processItem(LedgerBudgetGoalsEntry const& entry);
   void processItem(LedgerBudgetIncomeEntry const& entry);
   void processItem(LedgerBudgetReserveAmountEntry const& entry);
   void processItem(LedgerBudgetReservePercentEntry const& entry);
   void processItem(LedgerBudgetRoutineEntry const& entry);
   void processItem(LedgerBudgetWithholdingEntry const& entry);
   void processItem(LedgerTransaction const& transaction);
   bool processItem(LedgerTransactionV2 const& transaction);
   void processItem(LedgerTransactionV2AccountEntry const& entry);
   void processItem(LedgerTransactionV2CategoryEntry const& entry);
   void processItem(LedgerTransactionV2OwnerEntry const& entry);
   void setDateFormat(std::string const& dateFormat);
   void start();

private:
   std::string m_dateFormat = "yyyy/MM/dd";
   std::ofstream m_file;
   std::string m_fileName;
};
