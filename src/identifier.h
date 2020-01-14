#ifndef IDENTIFIER_H
#define IDENTIFIER_H

#include <QString>

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
   Identifier(QString const& value, Type type);

   Type type() const;

   operator QString() const;
   bool operator==(Identifier const& other) const;
   bool operator<(Identifier const& other) const;

private:
   Type m_type = Type::UNINITIALIZED;
   QString m_value;
};

#endif
