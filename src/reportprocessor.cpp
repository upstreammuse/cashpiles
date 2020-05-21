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

void ReportProcessor::start()
{
}
