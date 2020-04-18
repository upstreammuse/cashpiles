#include "ledgererror.h"

#include "itemprocessor.h"

LedgerError::LedgerError(std::string const& filename, size_t linenum) :
   LedgerItem(filename, linenum)
{
}

void LedgerError::processItem(ItemProcessor& processor) const
{
   processor.processItem(*this);
}

void LedgerError::setItem(std::shared_ptr<LedgerItem const> item)
{
   m_item = item;
}

void LedgerError::setMessage(std::string const& message)
{
   m_message = message;
}
