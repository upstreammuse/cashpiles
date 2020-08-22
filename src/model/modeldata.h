#pragma once

#include <string>

namespace model { struct ModelData; }

struct model::ModelData
{
   virtual ~ModelData();
   int refId = 0;  // FK to other data models

protected:
   int newId();

private:
   int m_nextId = 1;
};
