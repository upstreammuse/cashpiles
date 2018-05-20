#include "ledgeraccount.h"

void LedgerAccount::addBalance(int amount)
{
   m_balance += amount;
}

int LedgerAccount::balance() const
{
   return m_balance;
}

bool LedgerAccount::budget() const
{
   return m_budget;
}

void LedgerAccount::setBudget(bool budget)
{
   m_budget = budget;
}

QString LedgerAccount::name() const
{
   return m_name;
}

void LedgerAccount::setName(QString const& name)
{
   m_name = name;
}

void LedgerAccount::addTransferFrom(QString const& payee, int amount)
{
   m_transfersFrom[payee] += amount;
}

void LedgerAccount::addTransferTo(QString const& payee, int amount)
{
   m_transfersTo[payee] += amount;
}
