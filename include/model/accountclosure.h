#ifndef MODEL_ACCOUNTCLOSURE_H
#define MODEL_ACCOUNTCLOSURE_H

#include <QTextStream>

namespace model { struct AccountClosure; }

struct model::AccountClosure
{
   QString account;  //PK,FK
};

QTextStream& operator<<(QTextStream& out, model::AccountClosure const& data);
QTextStream& operator>>(QTextStream& in, model::AccountClosure& data);

#endif
