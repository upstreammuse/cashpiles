#include "transferbalancer.h"

#include "cashpiles.h"
#include "ledgeraccount.h"
#include "ledgerbudget.h"
#include "ledgerreserve.h"
#include "ledgertransaction.h"

void TransferBalancer::finish()
{
   checkTransfers(m_lastDate.addDays(1));
}

void TransferBalancer::processItem(LedgerAccount const& account)
{
   checkTransfers(account.date());
   m_accounts.insert(account.name());
}

void TransferBalancer::processItem(LedgerBudget const& budget)
{
   checkTransfers(budget.date());
}

void TransferBalancer::processItem(LedgerReserve const& reserve)
{
   checkTransfers(reserve.date());
}

void TransferBalancer::processItem(LedgerTransaction const& transaction)
{
   checkTransfers(transaction.date());
   foreach (LedgerTransactionEntry const& entry, transaction.entries())
   {
      // TODO this will become a check on the identifier to be an account name
      if (entry.transfer())
      {
         // TODO this goes away with Identifier integration
         Identifier i1(transaction.account(), Identifier::Type::ACCOUNT);
         Identifier i2(entry.payee(), Identifier::Type::ACCOUNT);

         m_accounts.insert(i1);
         m_accounts.insert(i2);
         m_transfers[i1][i2] += entry.amount();
      }
   }
}

void TransferBalancer::checkTransfers(QDate const& date)
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
               die(QString("Transfers between '%1' and '%2' do not match on "
                           "%3.  Mismatch of %4")
                   .arg(*it)
                   .arg(*it2)
                   .arg(m_lastDate.toString(m_dateFormat))
                   .arg(balance.toString()));
            }
         }
      }
      m_lastDate = date;
   }
}
