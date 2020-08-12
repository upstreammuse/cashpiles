#pragma once

#include <string>

class Date
{
   friend class DateBuilder;

public:
   static int daysInMonth(int month, int year);
   static Date fromString(std::string const& date, std::string const& format);
   static bool isLeapYear(int year);

public:
   int day() const;
   int month() const;
   int year() const;
   std::string toString(std::string const& format) const;

   Date addDays(int days) const;
   Date addMonths(int months) const;
   Date addYears(int years) const;
   int daysTo(Date const& other) const;

   bool operator==(Date const& other) const;
   bool operator!=(Date const& other) const;
   bool operator<(Date const& other) const;
   bool operator<=(Date const& other) const;
   bool operator>(Date const& other) const;

private:
   Date();
   void assertValid() const;
   int toJulianDayNumber() const;

private:
   int m_day = 0;
   int m_month = 0;
   int m_year = 0;
};

class DateBuilder
{
public:
   Date toDate() const;

public:
   DateBuilder& day(int);
   DateBuilder& month(int);
   DateBuilder& year(int);

private:
   int m_day;
   int m_month;
   int m_year;
};
