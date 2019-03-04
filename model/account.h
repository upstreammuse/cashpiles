#ifndef MODEL_ACCOUNT_H
#define MODEL_ACCOUNT_H

#include <QTextStream>

namespace model { struct Account; }

struct model::Account
{
   QString name;  //PK
   bool onBudget;
};

QTextStream& operator<<(QTextStream& out, model::Account const& data);
QTextStream& operator>>(QTextStream& in, model::Account& data);

#endif
