#include "reportitem.h"

size_t ReportItem::s_nextId = 1;

ReportItem::~ReportItem()
{
}

size_t ReportItem::id() const
{
   return m_id;
}
