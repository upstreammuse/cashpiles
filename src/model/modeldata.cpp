#include "modeldata.h"

using namespace model;

ModelData::~ModelData()
{
}

int ModelData::newId()
{
   return m_nextId++;
}
