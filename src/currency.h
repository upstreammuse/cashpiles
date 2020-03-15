#ifndef CURRENCY_H
#define CURRENCY_H

#include <QRegularExpression>

class DateRange;

class Currency
{
public:
   static Currency fromString(QString const& string, bool* ok = nullptr);
   static void normalize(Currency& a, Currency& b);

public:
   Currency();
   Currency amortize(DateRange const& total, DateRange const& partial) const;
   void clear();
   bool isNegative() const;
   bool isZero() const;
   QString toString() const;

   Currency operator-() const;
   Currency operator+(Currency const& other) const;
   Currency operator-(Currency const& other) const;
   Currency operator*(qint64 factor) const;
   Currency operator*(double factor) const;
   bool operator==(Currency const& other) const;
   bool operator!=(Currency const& other) const;
   Currency& operator+=(Currency const& other);
   Currency& operator-=(Currency const& other);

private:
   uint m_decimalPlaces = 0;
   qint64 m_value = 0;
};

#endif
