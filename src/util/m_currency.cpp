#include "m_currency.h"

#include <stdexcept>

using namespace util;

using std::logic_error;
using std::string;

Currency Currency::parseCurrency(string s, CurrencyFormat const& format)
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
