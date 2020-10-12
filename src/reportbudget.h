#pragma once

#include <vector>
#include "currency.h"
#include "daterange.h"
#include "reportitem.h"

class ReportProcessor;

class ReportBudgetEntry : public ReportItem
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

   void processItem(ReportProcessor& processor) const;

private:
   std::string m_category;
   Currency m_categoryStartBalance;
   std::string m_goal;
   Currency m_goalBalance;
};

class ReportBudgetCloseEntry : public ReportBudgetEntry
{
public:
   Currency availableStartBalance() const;
   Currency availableEndBalance() const;
   void setAvailableStartBalance(Currency const& balance);

   std::string category() const;
   void setCategory(std::string const& category);

   Currency categoryBalance() const;
   void setCategoryBalance(Currency const& balance);

   std::string owner() const;
   void setOwner(std::string const& owner);

   void processItem(ReportProcessor& processor) const;

private:
   Currency m_availableStartBalance;
   std::string m_category;
   Currency m_categoryBalance;
   std::string m_owner;
};

class ReportBudgetGoalEntry : public ReportBudgetEntry
{
public:
   ReportBudgetGoalEntry(Date const& date);

   std::string category() const;
   void setCategory(std::string const& category);

   std::string goal() const;
   void setGoal(std::string const& goal);

   Currency goalAmount() const;
   void setGoalAmount(Currency const& amount);

   Date goalDate() const;
   void setGoalDate(Date const& date);

   void processItem(ReportProcessor& processor) const;

private:
   std::string m_category;
   std::string m_goal;
   Currency m_goalAmount;
   Date m_goalDate;
};

class ReportBudgetGoalAllocationEntry : public ReportBudgetEntry
{
public:
   Currency balance() const;
   void setBalance(Currency const& balance);

   std::string category() const;
   void setCategory(std::string const& category);

   std::string goal() const;
   void setGoal(std::string const& goal);

   void processItem(ReportProcessor& processor) const;

private:
   Currency m_balance;
   std::string m_category;
   std::string m_goal;
};

class ReportBudgetReserveAllocationEntry : public ReportBudgetEntry
{
public:
   Currency balance() const;
   void setBalance(Currency const& balance);

   std::string category() const;
   void setCategory(std::string const& category);

   void processItem(ReportProcessor& processor) const;

private:
   Currency m_balance;
   std::string m_category;
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

   void processItem(ReportProcessor& processor) const;

private:
   std::string m_fileName;
   size_t m_lineNumber;
   std::string m_text;
};

class ReportBudget : public ReportItem
{
public:
   DateRange dateRange() const;
   void setDateRange(DateRange const& dateRange);

   void appendEntry(std::shared_ptr<ReportBudgetEntry> entry);
   void processItem(ReportProcessor& processor) const;

private:
   DateRange m_dateRange;
   std::vector<std::shared_ptr<ReportBudgetEntry>> m_entries;
};
