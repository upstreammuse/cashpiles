#pragma once

#include <vector>
#include "currency.h"
#include "daterange.h"
#include "report.h"

class ReportProcessor;

class ReportBudgetEntry : public Report
{
};

class ReportBudgetCancelEntry : public ReportBudgetEntry
{
public:
   std::string category() const;
   void setCategory(std::string const& category);

   Currency categoryStartBalance() const;
   Currency categoryEndBalance() const;
   void setCategoryStartBalance(Currency const& balance);

   std::string goal() const;
   void setGoal(std::string const& goal);

   Currency goalBalance() const;
   void setGoalBalance(Currency const& balance);

   void processReport(ReportProcessor& processor) const;

private:
   std::string m_category;
   Currency m_categoryStartBalance;
   std::string m_goal;
   Currency m_goalBalance;
};

class ReportBudgetWarningEntry : public ReportBudgetEntry
{
public:
   std::string fileName() const;
   void setFileName(std::string const& fileName);

   size_t lineNumber() const;
   void setLineNumber(size_t lineNumber);

   std::string text() const;
   void setText(std::string const& text);

   void processReport(ReportProcessor& processor) const;

private:
   std::string m_fileName;
   size_t m_lineNumber;
   std::string m_text;
};

class ReportBudget : public Report
{
public:
   DateRange dateRange() const;
   void setDateRange(DateRange const& dateRange);

   void appendEntry(std::shared_ptr<ReportBudgetEntry> entry);
   void processReport(ReportProcessor& processor) const;

private:
   DateRange m_dateRange;
   std::vector<std::shared_ptr<ReportBudgetEntry>> m_entries;
};
