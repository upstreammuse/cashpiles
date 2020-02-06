#pragma once

#include <QHash>
#include "identifier.h"
#include "itemprocessor.h"

class IPTransactionCategorizer : public ItemProcessor
{
public:
   void processItem(LedgerAccount const& account);
   void processItem(LedgerTransaction const& transaction);

private:
   void checkCreateAccount(Identifier const& account, QString const& filename,
                           uint linenum);

private:
   QHash<Identifier, bool> m_accounts;
};
