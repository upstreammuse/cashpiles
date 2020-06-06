#pragma once

class ReportAccount;
class ReportAccountEntry;
class ReportBudget;
class ReportBudgetCancelEntry;
class ReportBudgetCloseEntry;
class ReportBudgetGoalEntry;
class ReportBudgetGoalAllocationEntry;
class ReportBudgetReserveAllocationEntry;
class ReportBudgetWarningEntry;

class ReportProcessor
{
public:
   virtual ~ReportProcessor();
   virtual void finish();
   virtual bool processReport(ReportAccount const& account);
   virtual void processReport(ReportAccountEntry const& entry);
   virtual bool processReport(ReportBudget const& budget);
   virtual void processReport(ReportBudgetCancelEntry const& entry);
   virtual void processReport(ReportBudgetCloseEntry const& entry);
   virtual void processReport(ReportBudgetGoalEntry const& entry);
   virtual void processReport(ReportBudgetGoalAllocationEntry const& entry);
   virtual void processReport(ReportBudgetReserveAllocationEntry const& entry);
   virtual void processReport(ReportBudgetWarningEntry const& entry);
   virtual void start();
};
