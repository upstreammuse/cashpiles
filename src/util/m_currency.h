#pragma once

#include <cstddef>

namespace util { class Currency; }

class util::Currency
{
public:
   size_t decimalPlaces = 0;
   long long int value = 0;
};
