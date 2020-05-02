#include "iptransferbalancer.h"

#include <sstream>
#include "cashpiles.h"
#include "ledgeraccount.h"
#include "ledgerbudget.h"
#include "ledgertransaction.h"

void IPTransferBalancer::finish()
{
   checkTransfers(m_lastDate.addDays(1));
}

void IPTransferBalancer::processItem(LedgerAccount const& account)
{
   checkTransfers(account.date());
   m_accounts.insert(account.name());
}

void IPTransferBalancer::processItem(LedgerBudget const& budget)
{
   checkTransfers(budget.date());
}

void IPTransferBalancer::processItem(LedgerTransaction const& transaction)
{
   checkTransfers(transaction.date());
   for (LedgerTransactionEntry const& entry : transaction.entries())
   {
      switch (entry.payee().type())
      {
         case Identifier::Type::ACCOUNT:
            m_accounts.insert(transaction.account());
            m_accounts.insert(entry.payee());
            m_transfers[transaction.account()][entry.payee()] += entry.amount();
            break;
         case Identifier::Type::GENERIC:
            break;
         case Identifier::Type::UNINITIALIZED:
         case Identifier::Type::CATEGORY:
         case Identifier::Type::OWNER:
            die("Internal logic error, entry has wrong type");
      }
   }
}

void IPTransferBalancer::checkTransfers(Date const& date)
{
   if (date != m_lastDate)
   {
      for (auto it = m_accounts.cbegin(); it != m_accounts.cend(); ++it)
      {
         for (auto it2 = m_accounts.cbegin(); it2 != m_accounts.cend(); ++it2)
         {
            Currency balance = m_transfers[*it][*it2] + m_transfers[*it2][*it];
            if (!balance.isZero())
            {
               if (balance.isNegative())
               {
                  balance = -balance;
               }
               std::stringstream ss;
               ss << "Transfers between '" << *it << "' and '" << *it2
                  << "' do not match on " << m_lastDate.toString(m_dateFormat)
                  << ".  Mismatch of " << balance.toString();
               die(ss.str());
            }
         }
      }
      m_lastDate = date;
   }
}
