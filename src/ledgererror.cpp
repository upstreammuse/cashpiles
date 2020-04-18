#include "ledgererror.h"

#include "itemprocessor.h"

LedgerError::LedgerError(std::string const& filename, size_t linenum) :
   LedgerItem(filename, linenum)
{
}

std::shared_ptr<LedgerItem const> LedgerError::item() const
{
   return m_item;
}

void LedgerError::setItem(std::shared_ptr<LedgerItem const> item)
{
   m_item = item;
}

std::string LedgerError::message() const
{
   return m_message;
}

void LedgerError::setMessage(std::string const& message)
{
   m_message = message;
}

void LedgerError::processItem(ItemProcessor& processor) const
{
   processor.processItem(*this);
}
