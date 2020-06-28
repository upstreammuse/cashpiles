#pragma once

#include <string>

namespace model { class ModelReaderFormat; }

class model::ModelReaderFormat
{
public:
   explicit ModelReaderFormat(std::string const& dateFormat);
   size_t currencyDecimalDigits() const;
   std::string currencySymbol() const;
   std::string dateFormat() const;
   std::string decimalPoint() const;
   std::string groupSeparator() const;
   std::string negativeSign() const;

private:
   size_t m_currencyDecimalDigits;
   std::string m_currencySymbol;
   std::string m_dateFormat;
   std::string m_decimalPoint;
   std::string m_groupSeparator;
   std::string m_negativeSign;
};
