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

QString LedgerBudgetEntry::name() const
{
   return m_name;
}

QString LedgerBudgetEntry::owner() const
{
   return m_owner;
}

void LedgerBudgetEntry::setDate(QDate const& date)
{
   m_date = date;
}

void LedgerBudgetEntry::setName(QString const& name)
{
   m_name = name;
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

Interval LedgerBudgetReserveAmountEntry::interval() const
{
   return m_interval;
}

void LedgerBudgetReserveAmountEntry::processItem(ItemProcessor& processor) const
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

LedgerBudgetReservePercentEntry::LedgerBudgetReservePercentEntry(
      QString const& filename, uint lineNum) :
   LedgerBudgetEntry(filename, lineNum)
{
}

uint LedgerBudgetReservePercentEntry::percentage() const
{
   return m_percentage;
}

void LedgerBudgetReservePercentEntry::processItem(
      ItemProcessor& processor) const
{
   processor.processItem(*this);
}

void LedgerBudgetReservePercentEntry::setPercentage(uint percentage)
{
   m_percentage = percentage;
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
