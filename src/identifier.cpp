#include "identifier.h"

Identifier::Identifier()
{
}

Identifier::Identifier(QString const& value, Type type) :
   m_type(type),
   m_value(value)
{
   Q_ASSERT(type != Type::UNINITIALIZED);
}

Identifier::Type Identifier::type() const
{
   return m_type;
}

Identifier::operator QString() const
{
   Q_ASSERT(m_type != Type::UNINITIALIZED);
   return m_value;
}

bool Identifier::operator==(Identifier const& other) const
{
   Q_ASSERT(m_type == other.m_type);
   return m_value == other.m_value;
}

bool Identifier::operator!=(Identifier const& other) const
{
   Q_ASSERT(m_type == other.m_type);
   return m_value != other.m_value;
}

bool Identifier::operator<(Identifier const& other) const
{
   Q_ASSERT(m_type == other.m_type);
   return m_value < other.m_value;
}
