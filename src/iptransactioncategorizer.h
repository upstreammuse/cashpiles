#pragma once

#include <map>
#include "itemprocessor.h"

class IPTransactionCategorizer : public ItemProcessor
{
public:
   void processItem(LedgerAccount const& account);
   void processItem(LedgerTransaction const& transaction);
   void processItem(LedgerTransactionV2AccountEntry const& entry);
   void processItem(LedgerTransactionV2CategoryEntry const& entry);
   void processItem(LedgerTransactionV2OwnerEntry const& entry);

private:
   void checkCreateOff(std::string const& account, std::string const& filename,
                       size_t linenum);
   void checkCreateOn(std::string const& account, std::string const& filename,
                      size_t linenum);

private:
   std::map<std::string, bool> m_accounts;
};
