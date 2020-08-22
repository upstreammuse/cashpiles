#include "filereaderformat.h"

using std::string;

FileReaderFormat::FileReaderFormat(string const& dateFormat) :
   currencyFormat{currencyFormatFromLocale()},
   dateFormat{dateFormat}
{
}

CurrencyFormat FileReaderFormat::currencyFormatFromLocale()
{
   CurrencyFormat format;
   lconv* lc = localeconv();
   format.decimal = lc->mon_decimal_point ? lc->mon_decimal_point : ".";
   format.decimalDigits = static_cast<size_t>(lc->frac_digits);
   format.negative = lc->negative_sign ? lc->negative_sign : "-";
   format.separator = lc->mon_thousands_sep ? lc->mon_thousands_sep : ",";
   format.symbol = lc->currency_symbol ? lc->currency_symbol : "$";
   return format;
}
