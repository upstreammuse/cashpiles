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
   Date();
   Date(int year, int month, int day);
   Date addDays(long long int days) const;
   long long int daysTo(Date const& other) const;
   bool isNull() const;
   bool isValid() const;
   QDate toQDate() const;
   std::string toString(std::string const& format = "M/dd/yyyy") const;
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
