#include "ledgerreserve.h"

#include "itemprocessor.h"

LedgerReserve::LedgerReserve(QString const& filename, uint linenum) :
   LedgerItem(filename, linenum)
{
}

Currency LedgerReserve::amount() const
{
   return m_amount;
}

QString LedgerReserve::category() const
{
   return m_category;
}

QDate LedgerReserve::date() const
{
   return m_date;
}

void LedgerReserve::processItem(ItemProcessor& processor) const
{
   processor.processItem(*this);
}

void LedgerReserve::setAmount(Currency const& amount)
{
   m_amount = amount;
}

void LedgerReserve::setCategory(QString const& category)
{
   m_category = category;
}

void LedgerReserve::setDate(QDate const& date)
{
   m_date = date;
}
