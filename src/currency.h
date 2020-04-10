#ifndef CURRENCY_H
#define CURRENCY_H

#include <string>

class DateRange;

class Currency
{
public:
   static Currency fromString(std::string s, bool* ok = nullptr);
   static void normalize(Currency& a, Currency& b);

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
   unsigned int m_decimalPlaces = 0;
   long long int m_value = 0;
};

#endif
