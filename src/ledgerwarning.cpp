#include "ledgerwarning.h"

#include "itemprocessor.h"

std::string LedgerWarning::message() const
{
   return m_message;
}

void LedgerWarning::setMessage(std::string const& message, int itemId)
{
   m_itemId = itemId;
   m_message = message;
}

void LedgerWarning::processItem(ItemProcessor& processor) const
{
   processor.processItem(*this);
}
