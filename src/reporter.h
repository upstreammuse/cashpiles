#pragma once

#include <memory>
#include <vector>

class Report;
class ReportProcessor;

class Reporter
{
public:
   void appendReport(std::shared_ptr<Report> report);
   void processReports(ReportProcessor& processor);

private:
   std::vector<std::shared_ptr<Report>> m_reports;
};
