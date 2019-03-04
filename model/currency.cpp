#include "currency.h"

namespace
{
   model::Currency normalize(model::Currency const& a, uint decimalPlaces)
   {
      qint64 value = a.value;
      uint dp;
      for (dp = a.decimalPlaces; dp < decimalPlaces; ++dp)
      {
         value *= 10;
      }
      return
      {
         .value = value,
         .decimalPlaces = dp,
      };
   }
}

model::Currency model::Currency::operator+(Currency const& rhs) const
{
   Currency lhsN = normalize(*this, rhs.decimalPlaces);
   Currency rhsN = normalize(rhs, this->decimalPlaces);
   return
   {
      .value = lhsN.value + rhsN.value,
      .decimalPlaces = lhsN.decimalPlaces,
   };
}
