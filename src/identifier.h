#pragma once

#include <string>

class Identifier
{
public:
   enum class Type
   {
      UNINITIALIZED,
      ACCOUNT,
      CATEGORY,
      GENERIC,
      OWNER
   };

public:
   Identifier();
   Identifier(std::string const& value, Type type);

   bool isNotEmpty() const;
   Type type() const;

   bool operator==(Identifier const& other) const;
   bool operator!=(Identifier const& other) const;
   bool operator<(Identifier const& other) const;

   friend std::ostream& operator<<(std::ostream& out, Identifier const& ident);

private:
   Type m_type = Type::UNINITIALIZED;
   std::string m_value;
};

std::ostream& operator<<(std::ostream& out, Identifier const& ident);
