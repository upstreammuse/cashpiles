#include "reportbudget.h"

#include "reportprocessor.h"

void ReportBudget::processReport(ReportProcessor& processor) const
{
   processor.processReport(*this);
}
