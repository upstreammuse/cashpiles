#pragma once

#include <string>

namespace util { class Currency; }
namespace util { struct CurrencyFormat; }

struct util::CurrencyFormat
{
   std::string decimal;
   size_t decimalDigits;
   std::string negative;
   std::string separator;
   std::string symbol;
};

class util::Currency
{
public:
   static Currency parseCurrency(std::string, CurrencyFormat const&);

private:
   size_t m_decimalPlaces = 0;
   long long int m_value = 0;
};
