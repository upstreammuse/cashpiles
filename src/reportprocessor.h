#pragma once

class ReportAccount;
class ReportAccountEntry;
class ReportBudget;

class ReportProcessor
{
public:
   virtual ~ReportProcessor();
   virtual void finish();
   virtual bool processReport(ReportAccount const& account);
   virtual void processReport(ReportAccountEntry const& entry);
   virtual void processReport(ReportBudget const& budget);
   virtual void start();
};
