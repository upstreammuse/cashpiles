#include "iptransferbalancer.h"

#include "cashpiles.h"
#include "ledgeraccount.h"
#include "ledgerbudget.h"
#include "ledgerreserve.h"
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

void IPTransferBalancer::processItem(LedgerReserve const& reserve)
{
   checkTransfers(reserve.date());
}

void IPTransferBalancer::processItem(LedgerTransaction const& transaction)
{
   checkTransfers(transaction.date());
   foreach (LedgerTransactionEntry const& entry, transaction.entries())
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

void IPTransferBalancer::checkTransfers(QDate const& date)
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
                   .arg(QString::fromStdString(balance.toString())).toStdString());
            }
         }
      }
      m_lastDate = date;
   }
}
