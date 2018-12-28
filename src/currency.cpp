#include "currency.h"

#include <iostream>
#include <QLocale>

namespace
{
   Currency::CurrencyGroup const preload[] = {
      {"^(?<value>(\\-\\$|\\$\\-|\\$)\\d{1,3}(\\,\\d{3})*\\.\\d{2,})$", 2},
      {"^(?<value>(\\-\\$|\\$\\-|\\$)\\d{1,3}(\\,\\d{3})*)$", 0},
      {"^(?<value>(\\-\\$|\\$\\-|\\$)\\d{1,3}(\\d{3})*\\.\\d{2,})$", 2},
      {"^(?<value>(\\-\\$|\\$\\-|\\$)\\d{1,3}(\\d{3})*)$", 0}
   };
   size_t const preloadSize = sizeof(preload)/sizeof(Currency::CurrencyGroup);
}

QList<Currency::CompiledCurrencyGroup> Currency::m_currencies;

void Currency::initializeCurrencies()
{
   for (size_t i = 0; i < preloadSize; ++i)
   {
      addCurrency(preload[i]);
   }
}

void Currency::addCurrency(CurrencyGroup const& currencyGroup)
{
   QString rx(currencyGroup.regExp);
   rx.replace('$', QLocale::system().currencySymbol());
   rx.replace('-', QLocale::system().negativeSign());
   rx.replace(',', QLocale::system().groupSeparator());
   rx.replace('.', QLocale::system().decimalPoint());
   m_currencies.append(CompiledCurrencyGroup{QRegularExpression(rx),
                                             currencyGroup.decimalPlaces});
}

Currency::Currency()
{
}

Currency::Currency(QString const& asString, int lineNum)
{
   bool success = false;
   foreach (CompiledCurrencyGroup const& ccg, m_currencies)
   {
      QRegularExpressionMatch match(ccg.rx.match(asString));
      if (match.hasMatch())
      {
         m_value = match.captured("value")
                   .remove(QLocale::system().currencySymbol())
                   .remove(QLocale::system().groupSeparator())
                   .remove(QLocale::system().decimalPoint())
                   .toInt(&success);
         if (!success)
         {
            std::cerr << "Currency passed RX but failed integer conversion"
                      << std::endl;
         }
         m_decimalPlaces = ccg.decimalPlaces;
         break;
      }
   }
   if (!success)
   {
      std::cerr << "Unable to process '" << qPrintable(asString)
                << "' as currency, line " << lineNum << std::endl;
   }
}

bool Currency::isNegative() const
{
   return m_value < 0;
}

bool Currency::isZero() const
{
   return m_value == 0;
}

QString Currency::toString() const
{
   int value = m_value;
   QString output;
   if (value < 0)
   {
      value = -value;
      output += "-";
   }
   output += QLocale::system().toCurrencyString(
                value / pow(10, m_decimalPlaces));
   return output;
}

Currency Currency::operator+(Currency const& other) const
{
   Currency result(*this);
   result += other;
   return result;
}

Currency Currency::operator-(Currency const& other) const
{
   Currency result(*this);
   result -= other;
   return result;
}

CurrencySplit Currency::operator/(uint splits) const
{
   CurrencySplit result;
   result.amountA.m_decimalPlaces = m_decimalPlaces;
   result.amountB.m_decimalPlaces = m_decimalPlaces;

   result.amountB.m_value = m_value / splits;
   result.numSplitsB = splits;

   int difference = m_value - result.amountB.m_value * splits;
   if (difference == 0)
   {
      result.amountA.m_value = result.amountB.m_value;
      result.numSplitsA = splits;
      result.numSplitsB = 0;
   }
   else
   {
      result.amountA.m_value = result.amountB.m_value + 1;
      result.numSplitsA = difference;
      result.numSplitsB = splits - difference;
   }

   Q_ASSERT(result.amountA.m_value * result.numSplitsA +
            result.amountB.m_value * result.numSplitsB ==
            m_value);
   Q_ASSERT(result.amountA.m_decimalPlaces == m_decimalPlaces);
   Q_ASSERT(result.amountB.m_decimalPlaces == m_decimalPlaces);

   return result;
}

bool Currency::operator==(Currency const& other) const
{
   Currency a(*this);
   Currency b(other);
   normalize(a, b);
   return a.m_value == b.m_value;
}

bool Currency::operator!=(Currency const& other) const
{
   return !(*this == other);
}

Currency& Currency::operator+=(Currency const& other)
{
   Currency other_(other);
   normalize(*this, other_);
   m_value += other_.m_value;
   return *this;
}

Currency& Currency::operator-=(Currency const& other)
{
   Currency other_(other);
   normalize(*this, other_);
   m_value -= other_.m_value;
   return *this;
}

void Currency::normalize(Currency& a, Currency& b) const
{
   while (a.m_decimalPlaces < b.m_decimalPlaces)
   {
      a.m_value *= 10;
      a.m_decimalPlaces++;
   }
   while (b.m_decimalPlaces < a.m_decimalPlaces)
   {
      b.m_value *= 10;
      b.m_decimalPlaces++;
   }
}
