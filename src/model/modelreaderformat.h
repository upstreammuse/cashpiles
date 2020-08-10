#pragma once

#include <string>
#include "../currency.h"

namespace model { struct ModelReaderFormat; }

struct model::ModelReaderFormat
{
   CurrencyFormat currencyFormat;
   std::string dateFormat;

   explicit ModelReaderFormat(std::string const& dateFormat);
};
