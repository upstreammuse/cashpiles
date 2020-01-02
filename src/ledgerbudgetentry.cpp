#include "ledgerbudgetentry.h"

#include "itemprocessor.h"
#include "ledgerbudget.h"

LedgerBudgetEntry::~LedgerBudgetEntry()
{
}

QDate LedgerBudgetEntry::date() const
{
   return m_date;
}

QString LedgerBudgetEntry::name() const
{
   return m_name;
}

void LedgerBudgetEntry::setDate(QDate const& date)
{
   m_date = date;
}

void LedgerBudgetEntry::setName(QString const& name)
{
   m_name = name;
}

void LedgerBudgetGoalEntry::processItem(ItemProcessor& processor)
{
   processor.processItem(*this);
}

void LedgerBudgetIncomeEntry::processItem(ItemProcessor& processor)
{
   processor.processItem(*this);
}

Currency LedgerBudgetReserveAmountEntry::amount() const
{
   return m_amount;
}

Interval LedgerBudgetReserveAmountEntry::interval() const
{
   return m_interval;
}

void LedgerBudgetReserveAmountEntry::processItem(ItemProcessor& processor)
{
   processor.processItem(*this);
}

void LedgerBudgetReserveAmountEntry::setAmount(Currency const& amount)
{
   m_amount = amount;
}

void LedgerBudgetReserveAmountEntry::setInterval(Interval const& interval)
{
   m_interval = interval;
}

uint LedgerBudgetReservePercentEntry::percentage() const
{
   return m_percentage;
}

void LedgerBudgetReservePercentEntry::processItem(ItemProcessor& processor)
{
   processor.processItem(*this);
}

void LedgerBudgetReservePercentEntry::setPercentage(uint percentage)
{
   m_percentage = percentage;
}

void LedgerBudgetRoutineEntry::processItem(ItemProcessor& processor)
{
   processor.processItem(*this);
}
