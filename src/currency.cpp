#include "currency.h"

#include <cmath>
#include <QLocale>
#include "daterange.h"

QList<Currency::CompiledCurrencyGroup> Currency::m_currencies;

void Currency::addCurrency(QString const& regExp, uint decimalPlaces)
{
   QString rx(regExp);
   rx.replace('$', QLocale::system().currencySymbol());
   rx.replace('-', QLocale::system().negativeSign());
   rx.replace(',', QLocale::system().groupSeparator());
   rx.replace('.', QLocale::system().decimalPoint());
   m_currencies.append(
            CompiledCurrencyGroup{QRegularExpression(rx), decimalPlaces});
}

Currency Currency::fromString(QString const& string, bool* ok)
{
   Currency result;

   bool dummy;
   bool* success = ok ? ok : &dummy;
   *success = false;

   foreach (CompiledCurrencyGroup const& ccg, m_currencies)
   {
      QRegularExpressionMatch match(ccg.rx.match(string));
      if (match.hasMatch())
      {
         result.m_decimalPlaces = ccg.decimalPlaces;
         result.m_value = match.captured("value")
                          .remove(QLocale::system().currencySymbol())
                          .remove(QLocale::system().groupSeparator())
                          .remove(QLocale::system().decimalPoint())
                          .toLongLong(success);
         if (*success)
         {
            return result;
         }
      }
   }

   return result;
}

void Currency::initializeCurrencies()
{
   addCurrency(
            "^(?<value>(\\-\\$|\\$\\-|\\$)\\d{1,3}(\\,\\d{3})*\\.\\d{2,})$", 2);
   addCurrency("^(?<value>(\\-\\$|\\$\\-|\\$)\\d{1,3}(\\,\\d{3})*)$", 0);
   addCurrency("^(?<value>(\\-\\$|\\$\\-|\\$)\\d{1,3}(\\d{3})*\\.\\d{2,})$", 2);
   addCurrency("^(?<value>(\\-\\$|\\$\\-|\\$)\\d{1,3}(\\d{3})*)$", 0);
}

void Currency::normalize(Currency& a, Currency& b)
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

Currency::Currency()
{
}

Currency Currency::amortize(DateRange const& total,
                            DateRange const& partial) const
{
   // calculate the overlap between the total range and the partial range, and
   // bail out if there is no overlap
   DateRange overlap = total.intersect(partial);
   if (overlap.isNull())
   {
      return Currency();
   }

   // it's easier to work with positive values, reverse the sign later if needed
   qint64 value = isNegative() ? -m_value : m_value;

   // amountA is the first (higher) daily amortization value, and amountB the
   // second (lower) value
   Currency amountA;
   Currency amountB;
   amountA.m_decimalPlaces = m_decimalPlaces;
   amountB.m_decimalPlaces = m_decimalPlaces;

   // numberA is the number of days at amountA, and numberB is the number of
   // days at amountB
   qint64 numberA = 0;
   qint64 numberB = total.days();

   // try to evenly split the total value across the days, and if it doesn't
   // come out even, then determine how many days have to have a value 1 higher
   // to make up the remainder and give an exact answer
   amountB.m_value = value / numberB;
   qint64 difference = value - amountB.m_value * numberB;
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
      numberB -= difference;
   }

   // put the negative signs back
   if (isNegative())
   {
      amountA.m_value = -amountA.m_value;
      amountB.m_value = -amountB.m_value;
   }

   // make sure the splits add up to the original value
   Q_ASSERT(amountA.m_value * numberA + amountB.m_value * numberB == m_value);
   Q_ASSERT(amountA.m_decimalPlaces == m_decimalPlaces);
   Q_ASSERT(amountB.m_decimalPlaces == m_decimalPlaces);

   // number of days between the start of the total period and the start of the
   // overlap period, which will be days we skip amountA
   qint64 dayOffset = total.startDate().daysTo(overlap.startDate());

   // number of days we will use amountA
   qint64 daysOfA = std::min(std::max(qint64(0), numberA - dayOffset),
                             overlap.days());

   // the rest of the days we will use amountB
   qint64 daysOfB = overlap.days() - daysOfA;

   // the total for the days we are using
   Currency amortized = amountA * daysOfA + amountB * daysOfB;

   // the total for the days we are not using
   Currency unused = amountA * (numberA - daysOfA) +
                     amountB * (numberB - daysOfB);

   // make sure the splits add up to the original value
   Q_ASSERT(amortized.m_value + unused.m_value == m_value);

   return amortized;
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

QString Currency::toString() const
{
   qint64 value = m_value;
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

Currency Currency::operator*(qint64 factor) const
{
   Currency result(*this);
   result.m_value *= factor;
   return result;
}

Currency Currency::operator*(double factor) const
{
   Currency result(*this);
   result.m_value = qRound64(result.m_value * factor);
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
