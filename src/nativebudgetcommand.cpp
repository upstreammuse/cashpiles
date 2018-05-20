#include "nativebudgetcommand.h"

NativeBudgetCommand::NativeBudgetCommand(int item, int line) :
   NativeCommand(item, line)
{
}

int NativeBudgetCommand::allocations() const
{
   return m_categories.size();
}

QString NativeBudgetCommand::amount(int index) const
{
   return m_amounts[index];
}

void NativeBudgetCommand::appendAllocation(QString const& category,
                                           QString const& amount)
{
   m_categories.append(category);
   m_amounts.append(amount);
}

QString NativeBudgetCommand::category(int index) const
{
   return m_categories[index];
}

QString NativeBudgetCommand::date() const
{
   return m_date;
}

void NativeBudgetCommand::setDate(QString const& date)
{
   m_date = date;
}
