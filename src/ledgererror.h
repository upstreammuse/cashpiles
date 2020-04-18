#pragma once

#include "ledgeritem.h"

class LedgerError : public LedgerItem
{
public:
   LedgerError(std::string const& filename, size_t linenum);

   std::shared_ptr<LedgerItem const> item() const;
   void setItem(std::shared_ptr<LedgerItem const> item);

   std::string message() const;
   void setMessage(std::string const& message);

   void processItem(ItemProcessor& processor) const;

private:
   std::shared_ptr<LedgerItem const> m_item;
   std::string m_message;
};

template<typename T>
std::unique_ptr<LedgerError> makeError(T const& item,
                                       std::string const& message)
{
   std::unique_ptr<LedgerError> retval(
            std::make_unique<LedgerError>(item.fileName(), item.lineNum()));
   retval->setItem(std::make_unique<T>(item));
   retval->setMessage(message);
   return retval;
}