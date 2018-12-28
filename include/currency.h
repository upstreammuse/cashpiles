#ifndef CURRENCY_H
#define CURRENCY_H

#include <QList>
#include <QRegularExpression>
#include <QString>

struct CurrencySplit;

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
   void clear();
   bool isNegative() const;
   bool isZero() const;
   Currency percentage(int percent) const;
   QString toString() const;
   Currency operator+(Currency const& other) const;
   Currency operator-(Currency const& other) const;
   Currency operator*(int factor) const;
   CurrencySplit operator/(uint splits) const;
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

struct CurrencySplit
{
   Currency amountA;
   int numSplitsA;
   Currency amountB;
   int numSplitsB;
};

#endif
