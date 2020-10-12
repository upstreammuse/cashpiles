#pragma once

#include <memory>

class ReportProcessor;

class ReportItem
{
public:
   virtual ~ReportItem();
   size_t id() const;

   virtual void processItem(ReportProcessor& processor) const = 0;

private:
   static size_t s_nextId;

private:
   size_t m_id = s_nextId++;
};
