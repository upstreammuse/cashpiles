#ifndef NATIVEBUDGETCOMMAND_H
#define NATIVEBUDGETCOMMAND_H

#include <QStringList>
#include "nativecommand.h"

class NativeBudgetCommand : public NativeCommand
{
public:
   NativeBudgetCommand(int item, int line);
   int allocations() const;
   QString amount(int index) const;
   void appendAllocation(QString const& category, QString const& amount);
   QString category(int index) const;
   QString date() const;
   void setDate(QString const& date);

private:
   QStringList m_amounts;
   QStringList m_categories;
   QString m_date;
};

#endif
