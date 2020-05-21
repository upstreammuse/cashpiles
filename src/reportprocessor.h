#pragma once

class ReportAccount;
class ReportAccountEntry;

class ReportProcessor
{
public:
   virtual ~ReportProcessor();
   virtual void finish();
   virtual bool processReport(ReportAccount const& account);
   virtual void processReport(ReportAccountEntry const& entry);
   virtual void start();
};
