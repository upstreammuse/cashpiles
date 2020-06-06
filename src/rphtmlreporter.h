#pragma once

#include <fstream>
#include <string>
#include "currency.h"
#include "reportprocessor.h"

class Report;

class RPHtmlReporter : public ReportProcessor
{
public:
   RPHtmlReporter(std::string const& reportsDir,
                     std::string const& dateFormat);
   void finish();
   bool processReport(ReportAccount const& account);
   void processReport(ReportAccountEntry const& entry);
   bool processReport(ReportBudget const& budget);
   void processReport(ReportBudgetCancelEntry const& entry);
   void processReport(ReportBudgetCloseEntry const& entry);
   void processReport(ReportBudgetGoalEntry const& entry);
   void processReport(ReportBudgetWarningEntry const& entry);

private:
   std::string idStr(Report const& report);
   void removeById(Report const& report);

   std::string td(std::string const& s);
   std::string tr(std::string const& s);

private:
   std::ofstream m_accountFile;
   Currency m_balance;
   std::ofstream m_budgetFile;
   std::string m_dateFormat;
   std::string m_reportsDir;
};
