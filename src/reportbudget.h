#pragma once

#include "report.h"

class ReportProcessor;

class ReportBudget : public Report
{
public:
   void processReport(ReportProcessor& processor) const;
};
