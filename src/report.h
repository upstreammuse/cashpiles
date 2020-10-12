#pragma once

#include <memory>
#include <vector>

class ReportItem;
class ReportProcessor;

class Report
{
public:
   void appendItem(std::shared_ptr<ReportItem> item);
   void processItems(ReportProcessor& processor) const;

private:
   std::vector<std::shared_ptr<ReportItem>> m_items;
};
