#include "accountbalancer.h"

#include "kernel/ledgeraccountcommand.h"
#include "kernel/ledgerbudget.h"
#include "kernel/ledgertransaction.h"

AccountBalancer::AccountBalancer(QObject* parent) :
   ItemProcessor(parent)
{
}

void AccountBalancer::processItem(LedgerAccountCommand const& account)
{
   checkTransfers(account.date());

   switch (account.mode())
   {
      case LedgerAccountCommand::Mode::CLOSED:
         if (m_accounts.contains(account.account()))
         {
            if (!m_accounts[account.account()].balance.isZero())
            {
               emit message(account,
                            "Cannot close account with non-zero balance");
            }
            else
            {
               m_accounts.remove(account.account());
            }
         }
         else
         {
            emit message(account, "Cannot close account that was not open");
         }
         break;
      case LedgerAccountCommand::Mode::OFF_BUDGET:
         if (!m_accounts.contains(account.account()))
         {
            m_accounts[account.account()].onbudget = false;
         }
         else
         {
            emit message(account, "Cannot open account that was already open");
         }
         break;
      case LedgerAccountCommand::Mode::ON_BUDGET:
         if (!m_accounts.contains(account.account()))
         {
            m_accounts[account.account()].onbudget = true;
         }
         else
         {
            emit message(account, "Cannot open account that was already open");
         }
         break;
   }
}

void AccountBalancer::processItem(LedgerBudget const& budget)
{
   checkTransfers(budget.date());
}

void AccountBalancer::processItem(LedgerComment const&)
{
}

void AccountBalancer::processItem(LedgerTransaction const& transaction)
{
   checkTransfers(transaction.date());

   if (!m_accounts.contains(transaction.account()))
   {
      emit message(transaction,
                   QString("Transaction against unknown or closed account '%1'")
                   .arg(transaction.account()));
      return;
   }
   m_accounts[transaction.account()].balance += transaction.amount();
   if (transaction.hasBalance() &&
       transaction.balance() != m_accounts[transaction.account()].balance)
   {
      emit message(transaction,
                   QString("Transaction balance %1 incorrect, should be %2")
                   .arg(transaction.balance().toString())
                   .arg(m_accounts[transaction.account()].balance.toString()));
   }

   foreach (LedgerTransactionEntry const& entry, transaction.entries())
   {
      if (entry.transfer())
      {
         m_transfers[transaction.account()][entry.payee()] += entry.amount();
      }
   }
}

void AccountBalancer::stop()
{
   checkTransfers(QDate());

   for (auto it(m_accounts.cbegin()); it != m_accounts.cend(); ++it)
   {
      emit balance(it.key(), it->onbudget, it->balance);
   }
}

void AccountBalancer::checkTransfers(QDate const& date)
{
   if (date != m_lastDate)
   {
      for (auto it = m_accounts.cbegin(); it != m_accounts.cend(); ++it)
      {
         for (auto it2 = m_accounts.cbegin(); it2 != m_accounts.cend(); ++it2)
         {
            Currency balance = m_transfers[it.key()][it2.key()] +
                               m_transfers[it2.key()][it.key()];
            if (!balance.isZero())
            {
               if (balance.isNegative())
               {
                  balance = -balance;
               }
               emit message(QString("Transfers between '%1' and '%2' do not "
                                    "match as of %3.  Mismatch of %4")
                            .arg(it.key())
                            .arg(it2.key())
                            .arg(date.toString())
                            .arg(balance.toString()));
            }
         }
      }
      m_lastDate = date;
   }
}
