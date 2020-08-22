#pragma once

#include "../itemprocessor.h"

namespace model { struct Model; }
namespace model { class ModelBuilder; }

class model::ModelBuilder : public ItemProcessor
{
public:
   ModelBuilder(Model&);
   bool processItem(LedgerBudget const&);

private:
   Model& m_model;
};
