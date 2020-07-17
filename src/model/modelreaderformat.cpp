#include "modelreaderformat.h"

#include <clocale>

using namespace model;
using namespace util;

using std::string;

ModelReaderFormat::ModelReaderFormat(string const& dateFormat) :
   dateFormat(dateFormat)
{
   struct lconv* lc = localeconv();
   currencyFormat.decimal = lc->mon_decimal_point ? lc->mon_decimal_point : ".";
   currencyFormat.decimalDigits = static_cast<size_t>(lc->frac_digits);
   currencyFormat.negative = lc->negative_sign ? lc->negative_sign : "-";
   currencyFormat.separator =
         lc->mon_thousands_sep ? lc->mon_thousands_sep : ",";
   currencyFormat.symbol = lc->currency_symbol ? lc->currency_symbol : "$";
}
