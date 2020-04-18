#pragma once

#include "ledgeritem.h"

class LedgerWarning : public LedgerItem
{
public:
   LedgerWarning(std::string const& filename, size_t linenum);
   void processItem(ItemProcessor& processor) const;
   void setItem(std::shared_ptr<LedgerItem const> item);
   void setMessage(std::string const& message);

private:
   std::shared_ptr<LedgerItem const> m_item;
   std::string m_message;
};

template<typename T>
std::unique_ptr<LedgerWarning> makeWarning(T const& item,
                                           std::string const& message)
{
   std::unique_ptr<LedgerWarning> retval(
            std::make_unique<LedgerWarning>(item.fileName(), item.lineNum()));
   retval->setItem(std::make_unique<T>(item));
   retval->setMessage(message);
   return retval;
}
