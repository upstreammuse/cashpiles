#include "daterange.h"

DateRange::DateRange()
{
}

DateRange::DateRange(QDate const& start, QDate const& end) :
   m_interval(start, end),
   m_startDate(start)
{
}

DateRange::DateRange(QDate const& start, Interval const& interval) :
   m_interval(interval),
   m_startDate(start)
{
}

uint DateRange::days() const
{
   return m_startDate.daysTo(m_startDate + m_interval);
}

QDate DateRange::endDate() const
{
   return (m_startDate + m_interval).addDays(-1);
}

DateRange DateRange::intersect(DateRange const& other) const
{
   if (isNull() || other.isNull())
   {
      return DateRange();
   }

   QDate startDate = std::max(m_startDate, other.startDate());
   QDate endDate = std::min(this->endDate(), other.endDate());
   if (startDate <= endDate)
   {
      return DateRange(startDate, endDate);
   }
   else
   {
      return DateRange();
   }
}

bool DateRange::isNull() const
{
   return m_startDate.isNull();
}

QDate DateRange::startDate() const
{
   return m_startDate;
}

DateRange& DateRange::operator++()
{
   m_startDate = m_startDate + m_interval;
   return *this;
}

bool DateRange::operator==(DateRange const& other) const
{
   return m_startDate == other.m_startDate && endDate() == other.endDate();
}
