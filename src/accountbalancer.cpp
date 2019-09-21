#include "accountbalancer.h"

#include <QDebug>
#include "ledgeraccount.h"
#include "ledgerbudget.h"
#include "ledgertransaction.h"

void AccountBalancer::finish()
{
   checkTransfers(m_lastDate, "", 0);

   for (auto it(m_accounts.cbegin()); it != m_accounts.cend(); ++it)
   {
      qDebug() << QString("Account %1 balance = %2")
                  .arg(it.key())
                  .arg(it->balance.toString());
   }
}

void AccountBalancer::processItem(LedgerAccount const& account)
{
   checkTransfers(account.date(), account.fileName(), account.lineNum());

   switch (account.mode())
   {
      case LedgerAccount::Mode::CLOSED:
         if (m_accounts.contains(account.name()))
         {
            if (!m_accounts[account.name()].balance.isZero())
            {
               qDebug() << QString("File '%1', line %2: Cannot close account "
                                   "'%3' with non-zero balance")
                           .arg(account.fileName())
                           .arg(account.lineNum())
                           .arg(account.name());
            }
            else
            {
               m_accounts.remove(account.name());
            }
         }
         else
         {
            qDebug() << QString("File '%1', line %2: Cannot close account '%3' "
                                "that was not open")
                        .arg(account.fileName())
                        .arg(account.lineNum())
                        .arg(account.name());
         }
         break;
      case LedgerAccount::Mode::OFF_BUDGET:
         if (!m_accounts.contains(account.name()))
         {
            m_accounts[account.name()].onbudget = false;
         }
         else
         {
            qDebug() << QString("File '%1', line %2: Cannot open account '%3' "
                                "that was already open")
                        .arg(account.fileName())
                        .arg(account.lineNum())
                        .arg(account.name());
         }
         break;
      case LedgerAccount::Mode::ON_BUDGET:
         if (!m_accounts.contains(account.name()))
         {
            m_accounts[account.name()].onbudget = true;
         }
         else
         {
            qDebug() << QString("File '%1', line %2: Cannot open account '%3' "
                                "that was already open")
                        .arg(account.fileName())
                        .arg(account.lineNum())
                        .arg(account.name());
         }
         break;
   }
}

void AccountBalancer::processItem(LedgerBudget const& budget)
{
   checkTransfers(budget.date(), budget.fileName(), budget.lineNum());
}

void AccountBalancer::processItem(LedgerBudgetGoalEntry const&)
{
}

void AccountBalancer::processItem(LedgerBudgetIncomeEntry const&)
{
}

void AccountBalancer::processItem(LedgerBudgetReserveAmountEntry const&)
{
}

void AccountBalancer::processItem(LedgerBudgetReservePercentEntry const&)
{
}

void AccountBalancer::processItem(LedgerBudgetRoutineEntry const&)
{
}

void AccountBalancer::processItem(LedgerComment const&)
{
}

void AccountBalancer::processItem(LedgerTransaction const& transaction)
{
   checkTransfers(transaction.date(), transaction.fileName(),
                  transaction.lineNum());

   if (!m_accounts.contains(transaction.account()))
   {
      qDebug() << QString("File '%1', line %2: Transaction against unknown or "
                          "closed account '%3'")
                  .arg(transaction.fileName())
                  .arg(transaction.lineNum())
                  .arg(transaction.account());
      return;
   }
   m_accounts[transaction.account()].balance += transaction.amount();

   foreach (LedgerTransactionEntry const& entry, transaction.entries())
   {
      if (entry.transfer())
      {
         m_transfers[transaction.account()][entry.payee()] += entry.amount();
      }
   }

   if (transaction.hasBalance()
       && transaction.balance() != m_accounts[transaction.account()].balance)
   {
      qDebug() << QString("File '%1', line %2: Account balance %3 incorrect, "
                          "should be %4")
                  .arg(transaction.fileName())
                  .arg(transaction.lineNum())
                  .arg(transaction.balance().toString())
                  .arg(m_accounts[transaction.account()].balance.toString());
   }
}

void AccountBalancer::start()
{
}

void AccountBalancer::checkTransfers(QDate const& date, QString const& filename,
                                     uint lineNum)
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
               qDebug() << QString("File '%1', line %2: Transfers between '%3' "
                                   "and '%4' do not match as of %5.  Mismatch "
                                   "of %6")
                           .arg(filename)
                           .arg(lineNum)
                           .arg(it.key())
                           .arg(it2.key())
                           .arg(date.toString())
                           .arg(balance.toString());
            }
         }
      }
      m_lastDate = date;
   }
}
