#pragma once

#include "ledgeritem.h"

class LedgerWarning : public LedgerItem
{
public:
   std::string message() const;
   void setMessage(std::string const& message, int itemId);

   void processItem(ItemProcessor& processor) const;

private:
   int m_itemId;
   std::string m_message;
};
