#include "reportprocessor.h"

ReportProcessor::~ReportProcessor()
{
}

void ReportProcessor::finish()
{
}

bool ReportProcessor::processReport(ReportAccount const&)
{
   return true;
}

void ReportProcessor::processReport(ReportAccountEntry const&)
{
}

bool ReportProcessor::processReport(ReportBudget const&)
{
   return true;
}

void ReportProcessor::processReport(ReportBudgetCancelEntry const&)
{
}

void ReportProcessor::processReport(ReportBudgetCloseEntry const&)
{
}

void ReportProcessor::processReport(ReportBudgetGoalEntry const&)
{
}

void ReportProcessor::processReport(ReportBudgetWarningEntry const&)
{
}

void ReportProcessor::start()
{
}
