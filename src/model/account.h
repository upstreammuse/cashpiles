#pragma once

#include "modeldata.h"

namespace model { struct Account; }

struct model::Account : public ModelData
{
   std::string const name;  //PK
   bool open = false;

   explicit Account(std::string const&);
};
