#include "accountbalancer.h"

#include <QTextStream>
#include "cashpiles.h"
#include "ledgeraccount.h"
#include "ledgerbudget.h"
#include "ledgertransaction.h"
#include "texttable.h"

void AccountBalancer::finish()
{
   checkTransfers(m_lastDate.addDays(1));
   QTextStream out(stdout);

   TextTable table;
   table.appendColumn(0, "== ON-BUDGET ACCOUNT ==  ");
   table.appendColumn(1, "== BALANCE ==");
   Currency totalOn;
   for (auto it(m_accounts.cbegin()); it != m_accounts.cend(); ++it)
   {
      if (it->onBudget)
      {
         table.appendColumn(0, it.key() + "  ");
         table.appendColumn(1, it->balance.toString());
         totalOn += it->balance;
      }
   }
   table.appendColumn(0, "== TOTAL ==  ");
   table.appendColumn(1, totalOn.toString());
   table.setColumnAlignment(1, TextTable::Alignment::RightAlign);
   table.print(out);
   out << endl;

   table.clear();
   table.appendColumn(0, "== OFF-BUDGET ACCOUNT ==  ");
   table.appendColumn(1, "== BALANCE ==");
   Currency totalOff;
   for (auto it(m_accounts.cbegin()); it != m_accounts.cend(); ++it)
   {
      if (!it->onBudget)
      {
         table.appendColumn(0, it.key() + "  ");
         table.appendColumn(1, it->balance.toString());
         totalOff += it->balance;
      }
   }
   table.appendColumn(0, "== TOTAL ==  ");
   table.appendColumn(1, totalOff.toString());
   table.appendColumn(0, "== NET WORTH ==  ");
   table.appendColumn(1, (totalOn + totalOff).toString());
   table.setColumnAlignment(1, TextTable::Alignment::RightAlign);
   table.print(out);
   out << endl;
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
