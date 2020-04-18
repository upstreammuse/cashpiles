#include "ledgerwarning.h"

#include "itemprocessor.h"

LedgerWarning::LedgerWarning(std::string const& filename, size_t linenum) :
   LedgerItem(filename, linenum)
{
}

void LedgerWarning::processItem(ItemProcessor& processor) const
{
   processor.processItem(*this);
}

void LedgerWarning::setItem(std::shared_ptr<LedgerItem const> item)
{
   m_item = item;
}

void LedgerWarning::setMessage(std::string const& message)
{
   m_message = message;
}
