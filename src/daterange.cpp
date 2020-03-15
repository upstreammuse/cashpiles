#include "daterange.h"

#include <QDate>

DateRange::DateRange()
{
}

DateRange::DateRange(Date const& start, Date const& end) :
   m_interval(start.toQDate(), end.toQDate()),
   m_startDate(start)
{
}

DateRange::DateRange(Date const& start, Interval const& interval) :
   m_interval(interval),
   m_startDate(start)
{
}

long long int DateRange::days() const
{
   return m_startDate.daysTo(Date::fromQDate(m_startDate.toQDate() + m_interval));
}

Date DateRange::endDate() const
{
   return Date::fromQDate((m_startDate.toQDate() + m_interval).addDays(-1));
}

DateRange DateRange::intersect(DateRange const& other) const
{
   if (isNull() || other.isNull())
   {
      return DateRange();
   }

   Date startDate = std::max(m_startDate, other.startDate());
   Date endDate = std::min(this->endDate(), other.endDate());
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

Date DateRange::startDate() const
{
   return m_startDate;
}

DateRange& DateRange::operator++()
{
   m_startDate = Date::fromQDate(m_startDate.toQDate() + m_interval);
   return *this;
}

bool DateRange::operator==(DateRange const& other) const
{
   return m_startDate == other.m_startDate && endDate() == other.endDate();
}
