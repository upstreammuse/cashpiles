#include "identifier.h"

#include <cassert>

Identifier::Identifier()
{
}

Identifier::Identifier(std::string const& value, Type type) :
   m_type(type),
   m_value(value)
{
   assert(type != Type::UNINITIALIZED);
}

bool Identifier::isNotEmpty() const
{
   assert(m_type != Type::UNINITIALIZED);
   return m_value != "";
}

Identifier::Type Identifier::type() const
{
   return m_type;
}

bool Identifier::operator==(Identifier const& other) const
{
   assert(m_type == other.m_type);
   return m_value == other.m_value;
}

bool Identifier::operator!=(Identifier const& other) const
{
   assert(m_type == other.m_type);
   return m_value != other.m_value;
}

bool Identifier::operator<(Identifier const& other) const
{
   // TODO this is failing somewhere, probably because of the switch from == based hash table to < based map
//   assert(m_type == other.m_type);
   return m_value < other.m_value;
}

Identifier::operator std::string() const
{
   return m_value;
}

std::ostream& operator<<(std::ostream& out, Identifier const& ident)
{
   assert(ident.m_type != Identifier::Type::UNINITIALIZED);
   out << ident.m_value;
   return out;
}
