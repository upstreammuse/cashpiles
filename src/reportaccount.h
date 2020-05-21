#pragma once

#include <memory>
#include <vector>
#include "currency.h"
#include "daterange.h"
#include "report.h"

class ReportAccountEntry : public Report
{
public:
   Currency amount() const;
   void setAmount(Currency const& amount);

   bool cleared() const;
   void setCleared(bool cleared);

   Date date() const;
   void setDate(Date const& date);

   std::string text() const;
   void setText(std::string const& text);

   void processReport(ReportProcessor& processor) const;

private:
   Currency m_amount;
   bool m_cleared = false;
   Date m_date;
   std::string m_text;
};

class ReportAccount : public Report
{
public:
   std::string account() const;
   void setAccount(std::string const& account);

   Currency balanceStart() const;
   void setBalanceStart(Date const& date, Currency const& amount);
   Currency balanceEnd() const;
   void setBalanceEnd(Date const& date, Currency const& amount);

   DateRange dateRange() const;

   void appendEntry(std::shared_ptr<ReportAccountEntry> entry);
   void processReport(ReportProcessor& processor) const;

private:
   std::string m_account;
   Currency m_balanceEnd;
   Currency m_balanceStart;
   DateRange m_dateRange;
   std::vector<std::shared_ptr<ReportAccountEntry>> m_entries;
};
