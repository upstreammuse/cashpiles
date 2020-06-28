#include "modelreaderformat.h"

using namespace model;

using std::string;

ModelReaderFormat::ModelReaderFormat(string const& dateFormat) :
   m_dateFormat(dateFormat)
{
   struct lconv* lc = localeconv();
   m_currencyDecimalDigits = static_cast<size_t>(lc->frac_digits);
   m_currencySymbol = lc->currency_symbol ? lc->currency_symbol : "$";
   m_decimalPoint = lc->mon_decimal_point ? lc->mon_decimal_point : ".";
   m_groupSeparator = lc->mon_thousands_sep ? lc->mon_thousands_sep : ",";
   m_negativeSign = lc->negative_sign ? lc->negative_sign : "-";
}

size_t ModelReaderFormat::currencyDecimalDigits() const
{
   return m_currencyDecimalDigits;
}

string ModelReaderFormat::currencySymbol() const
{
   return m_currencySymbol;
}

string ModelReaderFormat::dateFormat() const
{
   return m_dateFormat;
}

string ModelReaderFormat::decimalPoint() const
{
   return m_decimalPoint;
}

string ModelReaderFormat::groupSeparator() const
{
   return m_groupSeparator;
}

string ModelReaderFormat::negativeSign() const
{
   return m_negativeSign;
}
