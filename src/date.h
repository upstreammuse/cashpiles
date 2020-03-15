#pragma once

class QDate;

class Date
{
public:
   static Date fromQDate(QDate const& date);

public:
   long long int daysTo(Date const& other) const;
   bool isNull() const;
   QDate toQDate() const;
   bool operator==(Date const& other) const;

private:
   long long int toJulianDayNumber() const;

private:
   int m_day;
   int m_month;
   int m_year;
};
