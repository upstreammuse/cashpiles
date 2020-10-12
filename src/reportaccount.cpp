#include "reportaccount.h"

#include "reportprocessor.h"

using std::string;

ReportAccountEntry::ReportAccountEntry(Date const& date) :
   m_date(date)
{
}

Currency ReportAccountEntry::amount() const
{
   return m_amount;
}

void ReportAccountEntry::setAmount(Currency const& amount)
{
   m_amount = amount;
}

bool ReportAccountEntry::cleared() const
{
   return m_cleared;
}

void ReportAccountEntry::setCleared(bool cleared)
{
   m_cleared = cleared;
}

Date ReportAccountEntry::date() const
{
   return m_date;
}

void ReportAccountEntry::setDate(Date const& date)
{
   m_date = date;
}

string ReportAccountEntry::text() const
{
   return m_text;
}

void ReportAccountEntry::setText(string const& text)
{
   m_text = text;
}

void ReportAccountEntry::processItem(ReportProcessor& processor) const
{
   processor.processReport(*this);
}

string ReportAccount::account() const
{
   return m_account;
}

void ReportAccount::setAccount(string const& account)
{
   m_account = account;
}

Currency ReportAccount::balanceStart() const
{
   return m_balanceStart;
}

void ReportAccount::setBalanceStart(Date const& date, Currency const& amount)
{
   if (m_dateRange.isNull())
   {
      m_dateRange = DateRange(date, date);
   }
   else if (m_dateRange.endDate() < date)
   {
      m_dateRange = DateRange(m_dateRange.startDate(), date);
   }
   m_balanceStart = amount;
}

Currency ReportAccount::balanceEnd() const
{
   return m_balanceEnd;
}

void ReportAccount::setBalanceEnd(Date const& date, Currency const& amount)
{
   if (m_dateRange.isNull())
   {
      m_dateRange = DateRange(date, date);
   }
   else if (m_dateRange.endDate() < date)
   {
      m_dateRange = DateRange(m_dateRange.startDate(), date);
   }
   m_balanceEnd = amount;
}

DateRange ReportAccount::dateRange() const
{
   return m_dateRange;
}

void ReportAccount::appendEntry(std::shared_ptr<ReportAccountEntry> entry)
{
   if (m_dateRange.isNull())
   {
      m_dateRange = DateRange(entry->date(), entry->date());
   }
   else if (m_dateRange.endDate() < entry->date())
   {
      m_dateRange = DateRange(m_dateRange.startDate(), entry->date());
   }
   m_entries.push_back(entry);
}

void ReportAccount::processItem(ReportProcessor& processor) const
{
   if (processor.processReport(*this))
   {
      for (auto entry : m_entries)
      {
         entry->processItem(processor);
      }
   }
}
