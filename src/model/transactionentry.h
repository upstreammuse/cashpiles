#pragma once

#include <memory>
#include "modeldata.h"

namespace model { struct Transaction; }
namespace model { struct TransactionEntry; }

struct model::TransactionEntry : public ModelData
{
   int const id = newId();  //PK
   std::shared_ptr<Transaction const> transaction;  //FK

   explicit TransactionEntry(std::shared_ptr<Transaction const>);
};
