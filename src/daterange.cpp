#include "daterange.h"

#include <QDate>

DateRange::DateRange()
{
}

DateRange::DateRange(QDate const& start, QDate const& end) :
   m_interval(start, end),
   m_startDate(Date::fromQDate(start))
{
}

DateRange::DateRange(QDate const& start, Interval const& interval) :
   m_interval(interval),
   m_startDate(Date::fromQDate(start))
{
}

qint64 DateRange::days() const
{
   return m_startDate.daysTo(Date::fromQDate(m_startDate.toQDate() + m_interval));
}

QDate DateRange::endDate() const
{
   return (m_startDate.toQDate() + m_interval).addDays(-1);
}

DateRange DateRange::intersect(DateRange const& other) const
{
   if (isNull() || other.isNull())
   {
      return DateRange();
   }

   QDate startDate = std::max(m_startDate.toQDate(), other.startDate());
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
   return m_startDate.toQDate();
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
