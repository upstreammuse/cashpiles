#include "model/accountclosure.h"

QTextStream& operator<<(QTextStream& out, model::AccountClosure const& data)
{
   out << data.account << '\n';
   return out;
}

QTextStream& operator>>(QTextStream& in, model::AccountClosure& data)
{
   data.account = in.readLine();
   return in;
}
