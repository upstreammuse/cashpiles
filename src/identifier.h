#ifndef IDENTIFIER_H
#define IDENTIFIER_H

#include <QString>

class Identifier
{
public:
   enum class Type
   {
      ACCOUNT,
      CATEGORY,
      GENERIC,
      OWNER
   };

public:
   Identifier(QString const& value, Type type);
   operator QString() const;
   Type type() const;

private:
   Type m_type;
   QString m_value;
};

#endif
