#pragma once

#include <string>
#include "../util/m_currency.h"

namespace model { struct ModelReaderFormat; }

struct model::ModelReaderFormat
{
   util::CurrencyFormat currencyFormat;
   std::string dateFormat;

   explicit ModelReaderFormat(std::string const& dateFormat);
};
