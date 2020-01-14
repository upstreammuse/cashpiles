#include "ledgerbudgetentry.h"

#include "itemprocessor.h"
#include "ledgerbudget.h"

LedgerBudgetEntry::LedgerBudgetEntry(QString const& filename, uint lineNum) :
   LedgerItem(filename, lineNum)
{
}

QDate LedgerBudgetEntry::date() const
{
   return m_date;
}

void LedgerBudgetEntry::setDate(QDate const& date)
{
   m_date = date;
}

QString LedgerBudgetEntry::name() const
{
   return m_name;
}

void LedgerBudgetEntry::setName(QString const& name)
{
   m_name = name;
}

QString LedgerBudgetEntry::owner() const
{
   return m_owner;
}

void LedgerBudgetEntry::setOwner(QString const& owner)
{
   m_owner = owner;
}

LedgerBudgetGoalEntry::LedgerBudgetGoalEntry(QString const& filename,
                                             uint lineNum) :
   LedgerBudgetEntry(filename, lineNum)
{
}

void LedgerBudgetGoalEntry::processItem(ItemProcessor& processor) const
{
   processor.processItem(*this);
}

LedgerBudgetIncomeEntry::LedgerBudgetIncomeEntry(QString const& filename,
                                                 uint lineNum) :
   LedgerBudgetEntry(filename, lineNum)
{
}

void LedgerBudgetIncomeEntry::processItem(ItemProcessor& processor) const
{
   processor.processItem(*this);
}

LedgerBudgetReserveAmountEntry::LedgerBudgetReserveAmountEntry(
      QString const& filename, uint lineNum) :
   LedgerBudgetEntry(filename, lineNum)
{
}

Currency LedgerBudgetReserveAmountEntry::amount() const
{
   return m_amount;
}

void LedgerBudgetReserveAmountEntry::setAmount(Currency const& amount)
{
   m_amount = amount;
}

Interval LedgerBudgetReserveAmountEntry::interval() const
{
   return m_interval;
}

void LedgerBudgetReserveAmountEntry::setInterval(Interval const& interval)
{
   m_interval = interval;
}

void LedgerBudgetReserveAmountEntry::processItem(ItemProcessor& processor) const
{
   processor.processItem(*this);
}

LedgerBudgetReservePercentEntry::LedgerBudgetReservePercentEntry(
      QString const& filename, uint lineNum) :
   LedgerBudgetEntry(filename, lineNum)
{
}

uint LedgerBudgetReservePercentEntry::percentage() const
{
   return m_percentage;
}

void LedgerBudgetReservePercentEntry::setPercentage(uint percentage)
{
   m_percentage = percentage;
}

void LedgerBudgetReservePercentEntry::processItem(
      ItemProcessor& processor) const
{
   processor.processItem(*this);
}

LedgerBudgetRoutineEntry::LedgerBudgetRoutineEntry(QString const& filename,
                                                   uint lineNum) :
   LedgerBudgetEntry(filename, lineNum)
{
}

void LedgerBudgetRoutineEntry::processItem(ItemProcessor& processor) const
{
   processor.processItem(*this);
}
