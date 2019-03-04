#ifndef CURRENCY_H
#define CURRENCY_H

#include <QString>

namespace model { struct Currency; }

struct model::Currency
{
   qint64 value;
   uint decimalPlaces;
   Currency operator+(Currency const& rhs) const;
};

#endif
