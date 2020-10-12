#include "reporter.h"

#include "reportitem.h"
#include "reportprocessor.h"

void Reporter::appendItem(std::shared_ptr<ReportItem> item)
{
   m_items.push_back(item);
}

void Reporter::processItems(ReportProcessor& processor) const
{
   processor.start();
   for (auto report : m_items)
   {
      report->processItem(processor);
   }
   processor.finish();
}
