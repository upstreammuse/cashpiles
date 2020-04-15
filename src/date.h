#pragma once

#include <string>

class Date
{
public:
   static Date currentDate();
   static Date fromString(std::string const& date, std::string const& format);

public:
   Date();
   Date(int year, int month, int day);
   Date addDays(int days) const;
   Date addMonths(int months) const;
   Date addYears(int years) const;
   long long int daysTo(Date const& other) const;
   bool isNull() const;
   bool isValid() const;
   int month() const;
   std::string toString(std::string const& format = "M/dd/yyyy") const;
   int year() const;
   bool operator==(Date const& other) const;
   bool operator!=(Date const& other) const;
   bool operator<(Date const& other) const;
   bool operator<=(Date const& other) const;
   bool operator>(Date const& other) const;

private:
   long long int toJulianDayNumber() const;

private:
   int m_day = 0;
   int m_month = 0;
   int m_year = 0;
};
