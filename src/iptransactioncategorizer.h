#pragma once

#include <map>
#include "identifier.h"
#include "itemprocessor.h"

class IPTransactionCategorizer : public ItemProcessor
{
public:
   void processItem(LedgerAccount const& account);
   void processItem(LedgerTransaction const& transaction);

private:
   void checkCreateAccount(Identifier const& account,
                           std::string const& filename, size_t linenum);

private:
   std::map<Identifier, bool> m_accounts;
};
