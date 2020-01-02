#include "ledgerreserve.h"

#include "itemprocessor.h"

LedgerReserve::LedgerReserve(QString const& filename, uint linenum) :
   LedgerItem(filename, linenum)
{
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
