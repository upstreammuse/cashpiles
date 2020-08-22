#pragma once

#include <string>
#include "currency.h"

struct FileReaderFormat
{
   CurrencyFormat const currencyFormat;
   std::string const dateFormat;

   explicit FileReaderFormat(std::string const& dateFormat);
   static CurrencyFormat currencyFormatFromLocale();
};
