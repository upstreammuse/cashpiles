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
   operator QString() const;
   Type type() const;

private:
   Type m_type = Type::UNINITIALIZED;
   QString m_value;
};

#endif
