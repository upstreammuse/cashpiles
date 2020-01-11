#include "identifier.h"

Identifier::Identifier(QString const& value, Type type) :
   m_type(type),
   m_value(value)
{
}

Identifier::operator QString() const
{
   return m_value;
}

Identifier::Type Identifier::type() const
{
   return m_type;
}
