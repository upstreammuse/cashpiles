#include "model/account.h"

QTextStream& operator<<(QTextStream& out, model::Account const& data)
{
   out << data.name << '\n'
       << (data.onBudget ? "on-budget" : "off-budget") << '\n';
   return out;
}

QTextStream& operator>>(QTextStream& in, model::Account& data)
{
   data.name = in.readLine();
   QString onBudget;
   in >> onBudget;
   data.onBudget = onBudget == "on-budget";
   return in;
}
