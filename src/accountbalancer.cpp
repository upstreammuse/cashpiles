#include "accountbalancer.h"

#include <QTextStream>
#include "cashpiles.h"
#include "ledgeraccount.h"
#include "ledgerbudget.h"
#include "ledgertransaction.h"

void AccountBalancer::finish()
{
   checkTransfers(m_lastDate.addDays(1));

   QString heading1a = "ON-BUDGET ACCOUNT";
   QString heading1b = "OFF-BUDGET ACCOUNT";
   QString heading2 = "BALANCE";
   int col1 = heading1b.size();
   int col2 = heading2.size();
   for (auto it(m_accounts.cbegin()); it != m_accounts.cend(); ++it)
   {
      col1 = std::max(it.key().size(), col1);
      col2 = std::max(it.value().balance.toString().size(), col2);
   }

   QTextStream out(stdout);
   out << heading1a.leftJustified(col1) << "  " << heading2.rightJustified(col2)
       << endl;
   for (auto it(m_accounts.cbegin()); it != m_accounts.cend(); ++it)
   {
      if (it->onBudget)
      {
         out << it.key().leftJustified(col1) << "  "
             << it->balance.toString().rightJustified(col2) << endl;
      }
   }
   out << heading1b.leftJustified(col1) << "  " << heading2.rightJustified(col2)
       << endl;
   for (auto it(m_accounts.cbegin()); it != m_accounts.cend(); ++it)
   {
      if (!it->onBudget)
      {
         out << it.key().leftJustified(col1) << "  "
             << it->balance.toString().rightJustified(col2) << endl;
      }
   }
}

void AccountBalancer::processItem(LedgerAccount const& account)
{
   checkTransfers(account.date());

   switch (account.mode())
   {
      case LedgerAccount::Mode::CLOSED:
         if (m_accounts.contains(account.name()))
         {
            if (!m_accounts[account.name()].balance.isZero())
            {
               die(account.fileName(), account.lineNum(),
                   QString("Cannot close account '%1' with non-zero balance")
                   .arg(account.name()));
            }
            else
            {
               m_accounts.remove(account.name());
            }
         }
         else
         {
            die(account.fileName(), account.lineNum(),
                QString("Cannot close account '%1' that was not open")
                .arg(account.name()));
         }
         break;
      case LedgerAccount::Mode::OFF_BUDGET:
         if (!m_accounts.contains(account.name()))
         {
            m_accounts[account.name()].onBudget = false;
            m_accounts[account.name()].balance = account.balance();
         }
         else
         {
            die(account.fileName(), account.lineNum(),
                QString("Cannot open account '%1' that was already open")
                .arg(account.name()));
         }
         break;
      case LedgerAccount::Mode::ON_BUDGET:
         if (!m_accounts.contains(account.name()))
         {
            m_accounts[account.name()].onBudget = true;
            m_accounts[account.name()].balance = account.balance();
         }
         else
         {
            die(account.fileName(), account.lineNum(),
                QString("Cannot open account '%1' that was already open")
                .arg(account.name()));
         }
         break;
   }
}

void AccountBalancer::processItem(LedgerBudget const& budget)
{
   checkTransfers(budget.date());
}

void AccountBalancer::processItem(LedgerTransaction const& transaction)
{
   checkTransfers(transaction.date());

   if (!m_accounts.contains(transaction.account()))
   {
      warn(transaction.fileName(), transaction.lineNum(),
           QString("Automatically opening on-budget account '%1'")
           .arg(transaction.account()));
      m_accounts[transaction.account()].onBudget = true;
   }
   m_accounts[transaction.account()].balance += transaction.amount();

   foreach (LedgerTransactionEntry const& entry, transaction.entries())
   {
      if (entry.transfer())
      {
         if (!m_accounts.contains(entry.payee()))
         {
            warn(transaction.fileName(), transaction.lineNum(),
                 QString("Automatically opening on-budget account '%1'")
                 .arg(entry.payee()));
            m_accounts[entry.payee()].onBudget = true;
         }

         if (m_accounts[transaction.account()].onBudget &&
             m_accounts[entry.payee()].onBudget &&
             entry.hasCategory())
         {
            die(transaction.fileName(), transaction.lineNum(),
                QString("Budget category set for transfer between on-budget "
                        "accounts '%1' and '%2'")
                .arg(transaction.account())
                .arg(entry.payee()));
         }
         else if (m_accounts[transaction.account()].onBudget &&
                  !m_accounts[entry.payee()].onBudget &&
                  !entry.hasCategory())
         {
            die(transaction.fileName(), transaction.lineNum(),
                QString("Missing budget category for transfer between "
                        "on-budget account '%1' and off-budget account '%2'")
                .arg(transaction.account())
                .arg(entry.payee()));
         }
         else if (!m_accounts[transaction.account()].onBudget &&
                  entry.hasCategory())
         {
            die(transaction.fileName(), transaction.lineNum(),
                QString("Budget category set for off-budget account '%1'")
                .arg(transaction.account()));
         }

         m_transfers[transaction.account()][entry.payee()] += entry.amount();
      }
      else
      {
         if (m_accounts[transaction.account()].onBudget && !entry.hasCategory())
         {
            die(transaction.fileName(), transaction.lineNum(),
                QString("Missing budget category for on-budget account '%1'")
                .arg(transaction.account()));
         }
         else if (!m_accounts[transaction.account()].onBudget &&
                  entry.hasCategory())
         {
            die(transaction.fileName(), transaction.lineNum(),
                QString("Budget category set for off-budget account '%1'")
                .arg(transaction.account()));
         }
      }
   }

   if (transaction.hasBalance()
       && transaction.balance() != m_accounts[transaction.account()].balance)
   {
      die(transaction.fileName(), transaction.lineNum(),
          QString("Account '%1' stated balance %2 does not match calculated "
                  "balance %3")
          .arg(transaction.account())
          .arg(transaction.balance().toString())
          .arg(m_accounts[transaction.account()].balance.toString()));
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
               die(QString("Transfers between '%1' and '%2' do not match as of "
                           "%3.  Mismatch of %4")
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
