#pragma once

#include <string>

class DateRange;

struct CurrencyFormat
{
   std::string decimal;
   size_t decimalDigits;
   std::string negative;
   std::string separator;
   std::string symbol;
};

class Currency
{
public:
   static Currency fromString(std::string s, bool* ok = nullptr);
   static Currency fromString(std::string, CurrencyFormat const&);
   static void normalize(Currency& a, Currency& b);
   static void normalize(Currency& curr, size_t decimalPlaces);

public:
   Currency();
   Currency amortize(DateRange const& total, DateRange const& partial) const;
   void clear();
   bool isNegative() const;
   bool isZero() const;
   std::string toString() const;

   Currency operator-() const;
   Currency operator+(Currency const& other) const;
   Currency operator-(Currency const& other) const;
   Currency operator*(long long int factor) const;
   Currency operator*(double factor) const;
   bool operator==(Currency const& other) const;
   bool operator!=(Currency const& other) const;
   Currency& operator+=(Currency const& other);
   Currency& operator-=(Currency const& other);

private:
   size_t m_decimalPlaces = 0;
   long long int m_value = 0;
};
