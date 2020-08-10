#include "currency.h"

#include <cassert>
#include <climits>
#include <clocale>
#include <cmath>
#include <cstring>
#include <stdexcept>
#include "daterange.h"

using std::logic_error;
using std::max;
using std::min;
using std::string;

Currency Currency::fromString(string s, bool* ok_)
{
   bool dummy;
   bool& ok = ok_ != nullptr ? *ok_ : dummy;
   ok = true;
   struct lconv* lc = localeconv();

   // remove currency symbol
   string symbol(lc->currency_symbol);
   size_t pos;
   if ((pos = s.find(symbol)) != string::npos)
   {
      s.erase(pos, symbol.size());
   }

   // remove separator symbols
   string sep(lc->mon_thousands_sep);
   for (pos = s.find(sep); pos != string::npos; pos = s.find(sep, pos))
   {
      s.erase(pos, sep.size());
   }

   // make sure we only have digits, -, and .
   string allowed("0123456789");
   allowed.append(lc->mon_decimal_point);
   allowed.append(lc->negative_sign);
   if (s.find_first_not_of(allowed) != string::npos)
   {
      ok = false;
      return Currency();
   }
   // this is a stupid hack, but when we split the number at the decimal, we end up reading "-0" when the number is less than a whole currency unit, and that gets translated into a positive number
   bool negate = false;
   if (atof(s.c_str()) < 0)
   {
      negate = true;
   }

   // get the numbers before and after the decimal
   string decimal(lc->mon_decimal_point);
   string before;
   string after;
   if ((pos = s.find(decimal)) != string::npos)
   {
      before = string(s, 0, pos);
      after = string(s, pos + 1);
   }
   else
   {
      before = s;
      after = "";
   }

   // make sure we either have no decimal, or the correct decimal digits
   if (after.size() != 0 && after.size() != size_t(lc->frac_digits))
   {
      ok = false;
      return Currency();
   }

   // turn the values into a currency class
   Currency retval;
   // make sure we are working with a positive number for now
   retval.m_value = strtoll(before.c_str(), nullptr, 10);
   if (retval.m_value < 0)
   {
      retval.m_value = -retval.m_value;
   }
   for (size_t i = 0; i < after.size(); ++i)
   {
      retval.m_value *= 10;
   }
   retval.m_value += strtoll(after.c_str(), nullptr, 10);
   if (negate)
   {
      retval.m_value = -retval.m_value;
   }
   retval.m_decimalPlaces = after.size();
   return retval;
}

Currency Currency::fromString(string s, CurrencyFormat const& format)
{
   // remove currency symbol
   auto pos = s.find(format.symbol);
   if (pos != string::npos)
   {
      s.erase(pos, format.symbol.size());
   }

   // remove separator symbols
   for (pos = s.find(format.separator); pos != string::npos;
        pos = s.find(format.separator, pos))
   {
      s.erase(pos, format.separator.size());
   }

   // make sure we only have digits, -, and .
   auto allowed = string { "0123456789" };
   allowed.append(format.decimal);
   allowed.append(format.negative);
   if (s.find_first_not_of(allowed) != string::npos)
   {
      throw logic_error("currency has invalid characters");
   }

   // this is a stupid hack, but when we split the number at the decimal, we
   // end up reading "-0" when the number is less than a whole currency unit,
   // and that gets translated into a positive number
   bool negate = strtod(s.c_str(), nullptr) < 0;

   // get the numbers before and after the decimal
   auto before = s;
   auto after = string { "" };
   pos = s.find(format.decimal);
   if (pos != string::npos)
   {
      before = s.substr(0, pos);
      after = s.substr(pos + format.decimal.size());
   }

   // make sure we either have no decimal, or the correct decimal digits
   if (after.size() != 0 && after.size() != format.decimalDigits)
   {
      throw logic_error("Currency has invalid number of decimal digits");
   }

   // turn the values into a currency class
   Currency retval;
   // make sure we are working with a positive number for now
   retval.m_value = strtoll(before.c_str(), nullptr, 10);
   if (retval.m_value < 0)
   {
      retval.m_value = -retval.m_value;
   }
   for (size_t i = 0; i < after.size(); ++i)
   {
      retval.m_value *= 10;
   }
   retval.m_value += strtoll(after.c_str(), nullptr, 10);
   if (negate)
   {
      retval.m_value = -retval.m_value;
   }
   retval.m_decimalPlaces = after.size();
   return retval;
}

void Currency::normalize(Currency& a, Currency& b)
{
   normalize(a, b.m_decimalPlaces);
   normalize(b, a.m_decimalPlaces);
}

void Currency::normalize(Currency& curr, size_t decimalPlaces)
{
   while (curr.m_decimalPlaces < decimalPlaces)
   {
      curr.m_value *= 10;
      curr.m_decimalPlaces++;
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
   long long int value = isNegative() ? -m_value : m_value;

   // amountA is the first (higher) daily amortization value, and amountB the
   // second (lower) value
   Currency amountA;
   Currency amountB;
   amountA.m_decimalPlaces = m_decimalPlaces;
   amountB.m_decimalPlaces = m_decimalPlaces;

   // numberA is the number of days at amountA, and numberB is the number of
   // days at amountB
   long long int numberA = 0;
   long long int numberB = total.days();

   // try to evenly split the total value across the days, and if it doesn't
   // come out even, then determine how many days have to have a value 1 higher
   // to make up the remainder and give an exact answer
   amountB.m_value = value / numberB;
   long long int difference = value - amountB.m_value * numberB;
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
   assert(amountA.m_value * numberA + amountB.m_value * numberB == m_value);
   assert(amountA.m_decimalPlaces == m_decimalPlaces);
   assert(amountB.m_decimalPlaces == m_decimalPlaces);

   // number of days between the start of the total period and the start of the
   // overlap period, which will be days we skip amountA
   long long int dayOffset = total.startDate().daysTo(overlap.startDate());

   // number of days we will use amountA
   long long int daysOfA = min(max(0LL, numberA - dayOffset), overlap.days());

   // the rest of the days we will use amountB
   long long int daysOfB = overlap.days() - daysOfA;

   // the total for the days we are using
   Currency amortized = amountA * daysOfA + amountB * daysOfB;

   // the total for the days we are not using
   Currency unused = amountA * (numberA - daysOfA) +
                     amountB * (numberB - daysOfB);

   // make sure the splits add up to the original value
   assert(amortized.m_value + unused.m_value == m_value);

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

// TODO why is C++ string handling so bad?
string Currency::toString() const
{
   struct lconv* lc = localeconv();
   size_t places = size_t(lc->frac_digits);

   auto curr = *this;
   normalize(curr, places);
   long long int value = curr.m_value;
   places = curr.m_decimalPlaces;

   char buffer[100] = "";
   char temp[100] = "";
   bool negate = false;
   if (m_value < 0)
   {
      value = -value;
      negate = true;
   }

   // prepend digits until we hit the decimal point
   for (size_t i = 0; i < places; ++i)
   {
      sprintf(temp, "%lld%s", value % 10, buffer);
      strcpy(buffer, temp);
      value /= 10;
   }
   // add the decimal if it should be displayed
   if (places != 0)
   {
      sprintf(temp, "%s%s", lc->mon_decimal_point, buffer);
      strcpy(buffer, temp);
   }
   // add a leading zero if we don't have any more digits to print and we had to
   // print a decimal point
   if (value <= 0 && places != 0)
   {
      sprintf(temp, "0%s", buffer);
      strcpy(buffer, temp);
   }

   char lastGroupSize = CHAR_MAX;
   // as long as we have specified digit groupings, print according to them
   for (char *i = lc->mon_grouping; *i != 0; ++i)
   {
      lastGroupSize = *i;
      for (char j = 0; j < lastGroupSize && value > 0; ++j)
      {
         sprintf(temp, "%lld%s", value % 10, buffer);
         strcpy(buffer, temp);
         value /= 10;
      }
      if (value > 0)
      {
         sprintf(temp, "%s%s", lc->mon_thousands_sep, buffer);
         strcpy(buffer, temp);
      }
   }
   // then continue on with the last specified group size
   while (value > 0)
   {
      for (char j = 0; j < lastGroupSize && value > 0; ++j)
      {
         sprintf(temp, "%lld%s", value % 10, buffer);
         strcpy(buffer, temp);
         value /= 10;
      }
      if (value > 0)
      {
         sprintf(temp, "%s%s", lc->mon_thousands_sep, buffer);
         strcpy(buffer, temp);
      }
   }

   // ignoring p_sign_posn to avoid dealing with parentheses
   sprintf(temp, "%s%s", lc->currency_symbol, buffer);
   strcpy(buffer, temp);
   if (negate)
   {
      sprintf(temp, "%s%s", lc->negative_sign, buffer);
      strcpy(buffer, temp);
   }

   return buffer;
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

Currency Currency::operator*(long long int factor) const
{
   Currency result(*this);
   result.m_value *= factor;
   return result;
}

Currency Currency::operator*(double factor) const
{
   Currency result(*this);
   result.m_value = llround(result.m_value * factor);
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
