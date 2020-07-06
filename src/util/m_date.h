#pragma once

#include <string>

namespace util { class Date; }

class util::Date
{
public:
   static unsigned char daysInMonth(unsigned char month, size_t year);
   static bool isLeapYear(size_t year);
   static Date parseDate(std::string const& date, std::string const& format);

public:
   Date(unsigned char month, unsigned char day, size_t year);
   unsigned char day() const;
   unsigned char month() const;
   size_t year() const;
   bool operator<(Date const&) const;
   bool operator==(Date const&) const;
   bool operator<=(Date const&) const;

private:
   unsigned char m_day;
   unsigned char m_month;
   size_t m_year;
};
