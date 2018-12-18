#ifndef CURRENCY_H
#define CURRENCY_H

#include <QList>
#include <QRegularExpression>
#include <QString>

class Currency
{
public:
   struct CurrencyGroup
   {
      QString regExp;
      int decimalPlaces;
   };

public:
   static void addCurrency(CurrencyGroup const& currencyGroup);
   static void initializeCurrencies();

public:
   Currency();
   Currency(QString const& asString, int lineNum);
   bool isZero() const;
   QString toString() const;
   Currency operator+(Currency const& other) const;
   Currency operator-(Currency const& other) const;
   bool operator==(Currency const& other) const;
   bool operator!=(Currency const& other) const;
   Currency& operator+=(Currency const& other);
   Currency& operator-=(Currency const& other);

private:
   struct CompiledCurrencyGroup
   {
      QRegularExpression rx;
      int decimalPlaces;
   };

private:
   void normalize(Currency& a, Currency& b) const;

private:
   static QList<CompiledCurrencyGroup> m_currencies;

private:
   int m_decimalPlaces = 0;
   int m_value = 0;
};

#endif