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

private:
   std::string idStr(Report const& report);
   void removeById(Report const& report);

private:
   Currency m_balance;
   std::string m_dateFormat;
   std::ofstream m_outFile;
   std::string m_reportsDir;
};
