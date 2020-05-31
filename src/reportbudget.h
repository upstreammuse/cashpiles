#pragma once

#include "daterange.h"
#include "report.h"

class ReportProcessor;

class ReportBudget : public Report
{
public:
   DateRange dateRange() const;
   void setDateRange(DateRange const& dateRange);

   void processReport(ReportProcessor& processor) const;

private:
   DateRange m_dateRange;
};
