#pragma once

namespace model { struct Date; }

struct model::Date
{
   long long int dayNumber;

   explicit Date(long long int);
};
