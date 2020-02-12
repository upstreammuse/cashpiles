#include "ledgerbudgetentry.h"

#include "itemprocessor.h"
#include "ledgerbudget.h"

LedgerBudgetEntry::LedgerBudgetEntry(QString const& filename, uint lineNum) :
   LedgerItem(filename, lineNum)
{
}

Identifier LedgerBudgetEntry::category() const
{
   return m_category;
}

void LedgerBudgetEntry::setCategory(Identifier const& category)
{
   Q_ASSERT(category.type() == Identifier::Type::CATEGORY);
   m_category = category;
}

QDate LedgerBudgetEntry::date() const
{
   return m_date;
}

void LedgerBudgetEntry::setDate(QDate const& date)
{
   Q_ASSERT(date.isValid());
   m_date = date;
}

Identifier LedgerBudgetEntry::owner() const
{
   return m_owner;
}

void LedgerBudgetEntry::setOwner(Identifier const& owner)
{
   Q_ASSERT(owner.type() == Identifier::Type::OWNER);
   m_owner = owner;
}

LedgerBudgetCloseEntry::LedgerBudgetCloseEntry(QString const& filename,
                                               uint lineNum) :
   LedgerBudgetEntry(filename, lineNum)
{
}

void LedgerBudgetCloseEntry::processItem(ItemProcessor& processor) const
{
   processor.processItem(*this);
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
