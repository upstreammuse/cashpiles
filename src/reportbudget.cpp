#include "reportbudget.h"

#include "reportprocessor.h"

DateRange ReportBudget::dateRange() const
{
   return m_dateRange;
}

void ReportBudget::setDateRange(DateRange const& dateRange)
{
   m_dateRange = dateRange;
}

void ReportBudget::processReport(ReportProcessor& processor) const
{
   processor.processReport(*this);
}
