#pragma once

#include <cstddef>

namespace model { struct Currency; }

struct model::Currency
{
   size_t decimalPlaces = 0;
   long long int value = 0;
};
