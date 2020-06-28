#pragma once

#include <string>

namespace model { struct ModelData; }

struct model::ModelData
{
   virtual ~ModelData();
   std::string note;

protected:
   int newId();

private:
   int m_nextId = 1;
};
