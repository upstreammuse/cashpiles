#include "reportbudget.h"

#include "reportprocessor.h"

using std::string;

string ReportBudgetCancelEntry::category() const
{
   return m_category;
}

void ReportBudgetCancelEntry::setCategory(string const& category)
{
   m_category = category;
}

Currency ReportBudgetCancelEntry::categoryStartBalance() const
{
   return m_categoryStartBalance;
}

Currency ReportBudgetCancelEntry::categoryEndBalance() const
{
   return m_categoryStartBalance + m_goalBalance;
}

void ReportBudgetCancelEntry::setCategoryStartBalance(Currency const& balance)
{
   m_categoryStartBalance = balance;
}

string ReportBudgetCancelEntry::goal() const
{
   return m_goal;
}

void ReportBudgetCancelEntry::setGoal(string const& goal)
{
   m_goal = goal;
}

Currency ReportBudgetCancelEntry::goalBalance() const
{
   return m_goalBalance;
}

void ReportBudgetCancelEntry::setGoalBalance(Currency const& balance)
{
   m_goalBalance = balance;
}

void ReportBudgetCancelEntry::processReport(ReportProcessor& processor) const
{
   processor.processReport(*this);
}

Currency ReportBudgetCloseEntry::availableStartBalance() const
{
   return m_availableStartBalance;
}

Currency ReportBudgetCloseEntry::availableEndBalance() const
{
   return m_availableStartBalance + m_categoryBalance;
}

void ReportBudgetCloseEntry::setAvailableStartBalance(Currency const& balance)
{
   m_availableStartBalance = balance;
}

string ReportBudgetCloseEntry::category() const
{
   return m_category;
}

void ReportBudgetCloseEntry::setCategory(string const& category)
{
   m_category = category;
}

Currency ReportBudgetCloseEntry::categoryBalance() const
{
   return m_categoryBalance;
}

void ReportBudgetCloseEntry::setCategoryBalance(Currency const& balance)
{
   m_categoryBalance = balance;
}

string ReportBudgetCloseEntry::owner() const
{
   return m_owner;
}

void ReportBudgetCloseEntry::setOwner(string const& owner)
{
   m_owner = owner;
}

void ReportBudgetCloseEntry::processReport(ReportProcessor& processor) const
{
   processor.processReport(*this);
}

string ReportBudgetWarningEntry::fileName() const
{
   return m_fileName;
}

void ReportBudgetWarningEntry::setFileName(string const& fileName)
{
   m_fileName = fileName;
}

size_t ReportBudgetWarningEntry::lineNumber() const
{
   return m_lineNumber;
}

void ReportBudgetWarningEntry::setLineNumber(size_t lineNumber)
{
   m_lineNumber = lineNumber;
}

string ReportBudgetWarningEntry::text() const
{
   return m_text;
}

void ReportBudgetWarningEntry::setText(string const& text)
{
   m_text = text;
}

void ReportBudgetWarningEntry::processReport(ReportProcessor& processor) const
{
   processor.processReport(*this);
}

DateRange ReportBudget::dateRange() const
{
   return m_dateRange;
}

void ReportBudget::setDateRange(DateRange const& dateRange)
{
   m_dateRange = dateRange;
}

void ReportBudget::appendEntry(std::shared_ptr<ReportBudgetEntry> entry)
{
   m_entries.push_back(entry);
}

void ReportBudget::processReport(ReportProcessor& processor) const
{
   if (processor.processReport(*this))
   {
      for (auto entry : m_entries)
      {
         entry->processReport(processor);
      }
   }
}
