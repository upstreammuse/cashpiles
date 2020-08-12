#include "daterange.h"

// TODO this is a temporary HACK
DateRange::DateRange() :
   m_interval(0, Interval::Period::DAYS),
   m_startDate(DateBuilder().month(1).day(1).year(1).toDate())
{
   // TODO this constructor is evil
}

DateRange::DateRange(Date const& start, Date const& end) :
   m_interval(start, end),
   m_startDate(start)
{
}

::DateRange::DateRange(Date const& start, Interval const& interval) :
   m_interval(interval),
   m_startDate(start)
{
}

long long int DateRange::days() const
{
   return m_startDate.daysTo(m_startDate + m_interval);
}

Date DateRange::endDate() const
{
   return (m_startDate + m_interval).addDays(-1);
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

// TODO this is a temporary HACK
bool DateRange::isNull() const
{
   Date magic = DateBuilder().month(1).day(1).year(1).toDate();
   return m_startDate == magic;
}

Date DateRange::startDate() const
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
