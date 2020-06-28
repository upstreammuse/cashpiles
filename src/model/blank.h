#pragma once

#include "modeldata.h"

namespace model { struct Blank; }

struct model::Blank : public ModelData
{
   int const id = newId();  //PK
};
