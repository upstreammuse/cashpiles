#pragma once

#include <memory>

class ReportProcessor;

class Report
{
public:
   virtual ~Report();
   size_t id() const;

   virtual void processReport(ReportProcessor& processor) const = 0;

private:
   static size_t s_nextId;

private:
   size_t m_id = s_nextId++;
};
