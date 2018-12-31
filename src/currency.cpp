#include "currency.h"

#include <iostream>
#include <QLocale>
#include "daterange.h"

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

Currency Currency::amortize(DateRange const& total,
                            DateRange const& partial) const
{
   DateRange overlap = total.intersect(partial);
   if (overlap.isNull())
   {
      return Currency();
   }

   int value = isNegative() ? -m_value : m_value;

   Currency amountA;
   Currency amountB;
   amountA.m_decimalPlaces = m_decimalPlaces;
   amountB.m_decimalPlaces = m_decimalPlaces;

   int numberA = 0;
   int numberB = total.days();
   amountB.m_value = value / numberB;

   int difference = value - amountB.m_value * numberB;
   if (difference == 0)
   {
      amountA.m_value = amountB.m_value;
      numberA = numberB;
      numberB = 0;
   }
   else
   {
      amountA.m_value = amountB.m_value + 1;
      numberA = difference;
      numberB = numberB - difference;
   }

   if (isNegative())
   {
      amountA.m_value = -amountA.m_value;
      amountB.m_value = -amountB.m_value;
   }

   Q_ASSERT(amountA.m_value * numberA + amountB.m_value * numberB == m_value);
   Q_ASSERT(amountA.m_decimalPlaces == m_decimalPlaces);
   Q_ASSERT(amountB.m_decimalPlaces == m_decimalPlaces);

   int dayOffset = total.startDate().daysTo(overlap.startDate());
   int dayCount = overlap.days();
   Currency result;
   while (dayOffset < numberA && dayCount > 0)
   {
      result += amountA;
      dayOffset++;
      dayCount--;
   }
   while (dayCount > 0)
   {
      result += amountB;
      dayOffset++;
      dayCount--;
   }
   return result;
}

void Currency::clear()
{
   *this = Currency();
}

bool Currency::isNegative() const
{
   return m_value < 0;
}

bool Currency::isZero() const
{
   return m_value == 0;
}

Currency Currency::percentage(int percent) const
{
   Currency result(*this);
   result.m_value = qRound(result.m_value * percent / 100.0);
   return result;
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

Currency Currency::operator-() const
{
   Currency result(*this);
   result.m_value = -result.m_value;
   return result;
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

Currency Currency::operator*(int factor) const
{
   Currency result(*this);
   result.m_value *= factor;
   return result;
}

#if 0
CurrencySplit Currency::operator/(uint splits) const
{
   int value = isNegative() ? -m_value : m_value;

   CurrencySplit result;
   result.amountA.m_decimalPlaces = m_decimalPlaces;
   result.amountB.m_decimalPlaces = m_decimalPlaces;

   result.amountB.m_value = value / splits;
   result.numSplitsB = splits;

   int difference = value - result.amountB.m_value * splits;
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

   if (isNegative())
   {
      result.amountA.m_value = -result.amountA.m_value;
      result.amountB.m_value = -result.amountB.m_value;
   }

   Q_ASSERT(result.amountA.m_value * result.numSplitsA +
            result.amountB.m_value * result.numSplitsB ==
            m_value);
   Q_ASSERT(result.amountA.m_decimalPlaces == m_decimalPlaces);
   Q_ASSERT(result.amountB.m_decimalPlaces == m_decimalPlaces);

   return result;
}
#endif

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
