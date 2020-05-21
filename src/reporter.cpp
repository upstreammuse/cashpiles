#include "reporter.h"

#include "report.h"
#include "reportprocessor.h"

void Reporter::appendReport(std::shared_ptr<Report> report)
{
   m_reports.push_back(report);
}

void Reporter::processReports(ReportProcessor& processor)
{
   processor.start();
   for (auto report : m_reports)
   {
      report->processReport(processor);
   }
   processor.finish();
}
