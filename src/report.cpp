#include "report.h"

size_t Report::s_nextId = 1;

Report::~Report()
{
}

size_t Report::id() const
{
   return m_id;
}
