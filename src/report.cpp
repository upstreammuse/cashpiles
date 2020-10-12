#include "report.h"

#include "reportitem.h"
#include "reportprocessor.h"

void Report::appendItem(std::shared_ptr<ReportItem> item)
{
   m_items.push_back(item);
}

void Report::processItems(ReportProcessor& processor) const
{
   processor.start();
   for (auto it : m_items)
   {
      it->processItem(processor);
   }
   processor.finish();
}
