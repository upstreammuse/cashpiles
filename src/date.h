#pragma once

#include <string>

class QDate;

class Date
{
public:
   static Date currentDate();
   static Date fromQDate(QDate const& date);
   static Date fromString(std::string const& date, std::string const& format);

public:
   long long int daysTo(Date const& other) const;
   bool isNull() const;
   bool isValid() const;
   QDate toQDate() const;
   bool operator==(Date const& other) const;

private:
   long long int toJulianDayNumber() const;

private:
   int m_day;
   int m_month;
   int m_year;
};
