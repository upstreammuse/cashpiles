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

Identifier::operator QString() const
{
   Q_ASSERT(m_type != Type::UNINITIALIZED);
   return m_value;
}

Identifier::Type Identifier::type() const
{
   Q_ASSERT(m_type != Type::UNINITIALIZED);
   return m_type;
}
